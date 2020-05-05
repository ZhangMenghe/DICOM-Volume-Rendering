package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.Utils.fileUtils;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.stub.StreamObserver;
import helmsley.vr.proto.datasetResponse.datasetInfo;
import helmsley.vr.proto.volumeResponse.volumeInfo;
import java.io.*;
import java.lang.ref.WeakReference;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class fileTransferClient {
    final static String TAG = "fileTransferClient";
    private static WeakReference<fileTransferClient> selfReference;
    private static boolean finished = false, finished_mask=false;

    private ManagedChannel mChannel;

    private final int CLIENT_ID = 1;
    private datasetInfo target_ds;
    private volumeInfo target_vol;
    private boolean vol_with_mask = true;
    private final WeakReference<Activity> activityReference;
    private final String target_root_dir;
    private List<datasetInfo> available_remote_datasets,
            available_local_datasets = new ArrayList<>();
    private List<configResponse.configInfo> available_config_files;
    private Map<String, List<volumeInfo>> local_dv_map = new HashMap<>();

    final private String local_index_filename;
    private boolean local_initialized = false;
    public fileTransferClient(Activity activity){
        activityReference = new WeakReference<Activity>(activity);
        target_root_dir = activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name);
        selfReference = new WeakReference<>(this);
        local_index_filename = target_root_dir + "/" + activityReference.get().getString(R.string.cf_config_name);
    }
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            Request req = Request.newBuilder().setClientId(CLIENT_ID).build();
            dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
            available_remote_datasets =  blockingStub.getAvailableDatasets(req).getDatasetsList();
            available_config_files = blockingStub.getAvailableConfigs(req).getConfigsList();
            return "";
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
    }
    public void SetupLocal(){
        if(local_initialized) return;
        List<String> lpc = fileUtils.readLines(local_index_filename);

        if(lpc.isEmpty()){
            Log.i(TAG, "=== SetupLocal: No existing index file exist====");
            return;
        }
        for(String line:lpc){
            Log.e(TAG, "===SetupLocal: read " + line);
            String[] info = line.split(",");
            if(info.length < 7) continue;
            //eg: "Larry Smarr" ,"2016-10-26" ,"Larry-2016-10-26-MRI"
            datasetInfo tinfo = datasetInfo.newBuilder().setPatientName(info[0]).setFolderName(info[2]).setDate(info[1]).build();
            // eg. "series_214_DYN_COR_VIBE_3_RUNS" 512, 512, 48, 243.10002131, 2, 2
            volumeInfo vol_info = volumeInfo.newBuilder()
                    .setFolderName(info[3])
                    .setFileNums(Integer.parseInt(info[6]))
                    .setImgHeight(Integer.parseInt(info[4]))
                    .setImgWidth(Integer.parseInt(info[5]))
                    .setVolThickness(Float.parseFloat(info[7]))
                    .setMaskAvailable(Integer.parseInt(info[9]) != 0)
                    .build();

            update_local_info(tinfo, vol_info);
        }
        local_initialized = true;
    }
    public List<configResponse.configInfo> getAvailableConfigFiles(){
        return available_config_files;
    }
    public List<datasetInfo> getAvailableDataset(boolean isLocal){
        return isLocal? available_local_datasets: available_remote_datasets;
    }
    public List<volumeInfo> getAvailableVolumes(String dataset_name, boolean isLocal){
        if(isLocal) return local_dv_map.get(dataset_name);

        //download
        for(datasetInfo dsInfo: available_remote_datasets){
            if(dsInfo.getFolderName().equals(dataset_name)){target_ds = dsInfo; break;}
        }
        Request req = Request.newBuilder().setClientId(CLIENT_ID).setReqMsg(dataset_name).build();
        dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
        return blockingStub.getVolumeFromDataset(req).getVolumesList();
    }
    public boolean deleteLocalData(String dsname, int pos){
        //delete from local list
        if(!local_dv_map.containsKey(dsname)) return false;
        List<volumeInfo> infolist = local_dv_map.get(dsname);
        volumeInfo rinfo = infolist.get(pos);
        if(rinfo == null) return false;

        //remove data directory
        File tar_ds_dir = Paths.get(target_root_dir, dsname).toFile();
        if(!tar_ds_dir.exists()) return false;
        if(!fileUtils.deleteDirectory(new File(tar_ds_dir, rinfo.getFolderName()))) return false;

        //remove from local index file
        //todo: flush if many changes, avoid write line by line
        List<String> lines = fileUtils.readLines(local_index_filename);
        for(String line : lines){
            String[] info = line.split(",");
            if(info[2].equals(dsname)
                    && info[3].equals(rinfo.getFolderName())){
                lines.remove(line);
                fileUtils.writeToFile(local_index_filename, lines);

                //update local data
                infolist.remove(pos);
                if(infolist.isEmpty()){
                    local_dv_map.remove(dsname);
                    for(datasetInfo dsinfo:available_local_datasets){
                        if(dsinfo.getFolderName().equals(dsname)) {available_local_datasets.remove(dsinfo); break;}
                    }
                }else local_dv_map.put(dsname, infolist);
                return true;
            }
        }
        return false;
    }

    public void Download(String ds_name, volumeInfo target_volume){
        target_vol = target_volume;
        if(!LoadDataFromLocal(ds_name + "/" + target_vol.getFolderName()))
            new GrpcTask(new DownloadDICOMRunnable(), mChannel, this).execute(Paths.get(target_ds.getFolderName(), target_volume.getFolderName()).toString());
    }
    private void DownloadMasks(String target_path){
        if(!target_vol.getMaskAvailable()) return;
        Log.e(TAG, "====Start to DownloadMasks: " + target_path );
        new GrpcTask(new DownloadMasksRunnable(), mChannel, this).execute(target_path);
    }

    private static class GrpcTask extends AsyncTask<String, Void, String> {
        private final GrpcRunnable grpcRunnable;
        private final ManagedChannel channel;
        private final WeakReference<fileTransferClient> activityReference;

        GrpcTask(GrpcRunnable grpcRunnable, ManagedChannel channel, fileTransferClient activity) {
            this.grpcRunnable = grpcRunnable;
            this.channel = channel;
            this.activityReference = new WeakReference<fileTransferClient>(activity);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                String folder_name = params[0];
                return this.grpcRunnable.run(folder_name, dataTransferGrpc.newBlockingStub(channel), dataTransferGrpc.newStub(channel));
            } catch (Exception e) {
                StringWriter sw = new StringWriter();
                PrintWriter pw = new PrintWriter(sw);
                e.printStackTrace(pw);
                pw.flush();
                return "===Failed... :\n" + sw;
            }
        }

        @Override
        protected void onPostExecute(String result) {
            fileTransferClient activity = activityReference.get();
            if (activity == null) {
                return;
            }
            grpcRunnable.onPostExecute(activity);
        }
    }

    private interface GrpcRunnable {
        /** Perform a grpcRunnable and return all the logs. */
        String run(String folder_name, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub) throws Exception;
        void onPostExecute(fileTransferClient activity);
    }
    private static class DownloadDICOMRunnable implements GrpcRunnable{
        private String target_path;
        @Override
        public String run(String folder_path, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub)
                throws Exception{
            target_path = folder_path;
            loadTextureOnly(blockingStub);
//            loadDCMImage(blockingStub);
            return "===Success!\n";
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
            activity.saveDCMI();
            activity.DownloadMasks(target_path);
        }
        private void loadTextureOnly(dataTransferGrpc.dataTransferBlockingStub blockingStub){
            RequestWholeVolume req = RequestWholeVolume.newBuilder().setClientId(1).setReqMsg(target_path).setUnitSize(2).build();
            Iterator<volumeWholeResponse> data_itor;
            data_itor = blockingStub.downloadVolume(req);
            int id = 0;
            while(data_itor.hasNext()){
                volumeWholeResponse data = data_itor.next();

                JNIInterface.JNIsendData(0, id, data.getData().size(), 2, data.getData().toByteArray());
                id++;
            }
        }
        private void loadDCMImage(dataTransferGrpc.dataTransferBlockingStub blockingStub){
            Request req = Request.newBuilder().setClientId(1).setReqMsg(target_path).build();
            Iterator<dcmImage> dcm_img_iterator;
            dcm_img_iterator = blockingStub.download(req);
            while(dcm_img_iterator.hasNext()){
                dcmImage img = dcm_img_iterator.next();
                //tackle with the image here
                // Log.e(TAG, "====img:" + img.getPosition() );
                JNIInterface.JNIsendData(0, img.getDcmID(), img.getData().size(), 2, img.getData().toByteArray());
            }
        }
    }
    private static class DownloadMasksRunnable implements GrpcRunnable{
        @Override
        public String run(String folder_name, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub)
                throws Exception{
            loadTextureOnly(folder_name, asyncStub);
            return "success";
        }
        private void loadTextureOnly(String folder_name, dataTransferGrpc.dataTransferStub asyncStub){
            Request req = Request.newBuilder().setClientId(1).setReqMsg(folder_name).build();
            StreamObserver<volumeWholeResponse> mask_observer = new StreamObserver<volumeWholeResponse>() {
                int id = 0;
                @Override
                public void onNext(volumeWholeResponse value) {
                    JNIInterface.JNIsendData(1, id, value.getData().size(), 2, value.getData().toByteArray());
                    id +=1 ;
                }

                @Override
                public void onError(Throwable t) {
                    Log.i(TAG, "==============error========= " );

                }

                @Override
                public void onCompleted() {
                    Log.i(TAG, "==============Finish Loading Masks========= " );
                    selfReference.get().SaveMasks();
                    finished_mask = true;
                    id = 0;
                }
            };
            asyncStub.downloadMasksVolume(req, mask_observer);
        }
        private void loadMaskAsDCMIImage(String folder_name, dataTransferGrpc.dataTransferStub asyncStub){
            Request req = Request.newBuilder().setClientId(1).setReqMsg(folder_name).build();

            StreamObserver<dcmImage> mask_observer = new StreamObserver<dcmImage>() {
                @Override
                public void onNext(dcmImage value) {
                    Log.e(TAG, "==========onNext: "+ value.getPosition() );
                    JNIInterface.JNIsendData(1, value.getDcmID(), value.getData().size(), 2, value.getData().toByteArray());
                }

                @Override
                public void onError(Throwable t) {
                }

                @Override
                public void onCompleted() {
                    Log.i(TAG, "==============Finish Loading Masks========= " );
                    selfReference.get().SaveMasks();
                    finished_mask = true;
                }
            };
            asyncStub.downloadMasks(req, mask_observer);
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
        }
    }

    private void saveData(Activity activity, String fname) throws IOException{
        //save data
        String file_name = target_vol.getMaskAvailable()? activity.getString(R.string.cf_dcmwmask_name):fname;
        File tar_ds_dir = Paths.get(target_root_dir, target_ds.getFolderName()).toFile();
        if(!tar_ds_dir.exists()) tar_ds_dir.mkdir();

        //check relevant sub dirs
        String[] vol_dir_tree = target_vol.getFolderName().split("/");
        String vol_dir_prefix="";
        for(String vol_dir:vol_dir_tree){
            vol_dir_prefix = vol_dir_prefix+vol_dir;
            File tar_vol_dir = new File(tar_ds_dir, vol_dir_prefix);
            if(!tar_vol_dir.exists()) tar_vol_dir.mkdir();
        }

        File tar_vol_dir = new File(tar_ds_dir, target_vol.getFolderName());
        if(!tar_vol_dir.exists()) tar_vol_dir.mkdir();
        File datafile = new File(tar_vol_dir, file_name);

        saveLargeImageToFile(new FileOutputStream(datafile), JNIInterface.JNIgetVolumeData());
    }

    //save after download complete
    private void SaveMasks(){
        dialogUIs.FinishMaskLoading();
        Activity activity = activityReference.get();
        if(!Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))) return;

        try {
            saveData(activity, activity.getString(R.string.cf_dcmmask_name));
        } catch (Exception e) {
            Log.e(TAG, "====Failed to Save Masks to file");
        }
    }
    //save after download complete
    private void saveDCMI(){
        Activity activity = activityReference.get();
        //update local data
        update_local_info(target_ds, target_vol);
        dialogUIs.local_dirty = true;

        //save to local file
        if(Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))){
            try{
                String content = target_ds.getPatientName()+","
                        +target_ds.getDate()+","
                        +target_ds.getFolderName()+","
                        +target_vol.getFolderName()+","
                        +target_vol.getImgHeight()+","
                        +target_vol.getImgWidth()+","
                        +target_vol.getFileNums() + ","
                        +target_vol.getVolThickness()+","
                        +(target_vol.getMaskAvailable()?"2,2":"2,0")
                        +"\n";
                fileUtils.addToFile(local_index_filename, content);
                saveData(activity, activity.getString(R.string.cf_dcm_name));
            }catch (Exception e){
                e.printStackTrace();

                Log.e(TAG, "====Failed to Save Results to file");
            }
        }
        finished = true;
    }
    private void update_local_info(datasetInfo tds, volumeInfo tvol){
        String dsname = tds.getFolderName();

        if(!local_dv_map.containsKey(dsname)){
            available_local_datasets.add(tds);
            local_dv_map.put(dsname, new ArrayList<>());
        }
        List<volumeInfo> infolist = local_dv_map.get(dsname);
        for(volumeInfo info:infolist){
            if(info.getFolderName().equals(tvol.getFolderName())) return;
        }
        infolist.add(tvol);
        local_dv_map.put(dsname, infolist);
    }
    private boolean LoadDataFromLocal(String vpath){
        Activity activity = activityReference.get();
        //target folder exist
        Path destDir_path = Paths.get(target_root_dir, vpath);
        File destDir = destDir_path.toFile();

        if(!destDir.exists()) return false;

        //load data
        File DwM = new File(destDir, activity.getString(R.string.cf_dcmwmask_name));
        try{

            if(DwM.exists()){
                loadVolumeData(new FileInputStream(DwM), 2, 4);
                finished = true; finished_mask = true;
            }else{
                //load data and path separately
                File data = new File(destDir, activity.getString(R.string.cf_dcm_name));
                loadVolumeData(new FileInputStream(data), 0, 2);
                finished = true;
//
            }
        }catch(Exception e){
            e.printStackTrace();
            return false;
        }
        if(!DwM.exists()){
            try{
                File mask = new File(destDir, activity.getString(R.string.cf_dcmmask_name));
                loadVolumeData(new FileInputStream(mask), 1, 2);
            }catch (Exception e){
                e.printStackTrace();
                vol_with_mask = false;
            }
        }
        finished_mask = true;
        return true;
    }

    //2 for dcm+mask, usually 4 bits,
    // 0 for dcmi, 1 for mask, unit_size for single type usually 2, together will be 4
    private void loadVolumeData(InputStream instream, int target, int unit_size)
        throws IOException{
            byte[] chunk = new byte[1024];
            int id = 0;
            int len;
            while ((len = instream.read(chunk)) != -1) {
                    JNIInterface.JNIsendData(target, id, len, unit_size, chunk);
                    id++;
            }
    }

    private void saveLargeImageToFile(OutputStream ostream, byte[] data){
        try{
            ostream.write(data);
            ostream.flush();
            ostream.close();
        }catch (IOException e){
            e.printStackTrace();
            Log.e(TAG, "====Failed to Save Large Image to file");
        }
    }
    public void Shutdown() throws InterruptedException {
        mChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }
    public boolean isDownloadingProcessFinished(){
        return finished;
    }
    public void Reset(){
        finished = false;
    }
    public boolean isDownloadingMaskProcessFinished(){
        return finished_mask;
    }
    public void ResetMast(){
        finished_mask = false;
    }

}

package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.renderscript.ScriptGroup;
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
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.TimeUnit;


public class fileTransferClient {
    final static String TAG = "fileTransferClient";
    private static WeakReference<fileTransferClient> selfReference;
    public static boolean finished = false, finished_mask=false;

    private ManagedChannel mChannel;

    private final int CLIENT_ID = 1;
    private datasetInfo target_ds;
    private volumeInfo target_vol;

    private final WeakReference<Activity> activityReference;
    private final String target_root_dir;
    private List<datasetInfo> available_remote_datasets, available_local_datasets;
    private Map<String, List<volumeInfo>> local_dv_map = new HashMap<>();

    final private String local_index_filename;
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
        List<String> lpc = fileUtils.readLines(local_index_filename);
        available_local_datasets = new ArrayList<>();

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
            // eg. "series_214_DYN_COR_VIBE_3_RUNS" 512, 512, 48,2,2
            volumeInfo vol_info = volumeInfo.newBuilder().setFolderName(info[3]).setFileNums(Integer.parseInt(info[6])).setImgHeight(Integer.parseInt(info[4])).setImgWidth(Integer.parseInt(info[5])).build();

            update_local_info(tinfo, vol_info);
        }
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

    public void Download(String ds_name, volumeInfo target_volume){
        target_vol = target_volume;
        if(!LoadDataFromLocal(ds_name + "/" + target_vol.getFolderName()))
            new GrpcTask(new DownloadDICOMRunnable(), mChannel, this).execute(Paths.get(target_ds.getFolderName(), target_volume.getFolderName()).toString());
    }
    public void DownloadMasks(String target_path){
        Log.e(TAG, "====DownloadMasks: " + target_path );
        new GrpcTask(new DownloadMasksRunnable(), mChannel, this).execute(target_path);
    }

    private static class GrpcTask extends AsyncTask<String, Void, String> {
        private final GrpcRunnable grpcRunnable;
        private final ManagedChannel channel;
        private final WeakReference<fileTransferClient> activityReference;
        private String folder_name;

        GrpcTask(GrpcRunnable grpcRunnable, ManagedChannel channel, fileTransferClient activity) {
            this.grpcRunnable = grpcRunnable;
            this.channel = channel;
            this.activityReference = new WeakReference<fileTransferClient>(activity);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                folder_name = params[0];
                String log_msg = this.grpcRunnable.run(folder_name, dataTransferGrpc.newBlockingStub(channel), dataTransferGrpc.newStub(channel));
                return log_msg;
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
            Request req = Request.newBuilder().setClientId(1).setReqMsg(target_path).build();
            Iterator<dcmImage> dcm_img_iterator;
            dcm_img_iterator = blockingStub.download(req);
            while(dcm_img_iterator.hasNext()){
                dcmImage img = dcm_img_iterator.next();
                //tackle with the image here
                // Log.e(TAG, "====img:" + img.getPosition() );
                JNIInterface.JNIsendDCMImg(img.getDcmID(), 0, img.getData().toByteArray());
            }
            return "===Success!\n";
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
            activity.saveDCMIData();
            activity.DownloadMasks(target_path);
        }
    }
    private static class DownloadMasksRunnable implements GrpcRunnable{
        @Override
        public String run(String folder_name, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub)
                throws Exception{
//            ArrayList<dcmImage> lst = new ArrayList<>();
            Request req = Request.newBuilder().setClientId(1).setReqMsg(folder_name).build();

            StreamObserver<dcmImage> mask_observer = new StreamObserver<dcmImage>() {
                @Override
                public void onNext(dcmImage value) {
                    Log.e(TAG, "==========onNext: "+ value.getPosition() );
                    JNIInterface.JNIsendDCMIMask(value.getDcmID(), 0, value.getData().toByteArray());
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
            return "success";
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
        }
    }

    public void SaveMasks(){
        dialogUIs.FinishMaskLoading();
        File destDir = Paths.get(target_root_dir, target_ds.getFolderName(), target_vol.getFolderName()).toFile();
        if(!destDir.exists()) destDir.mkdirs();

        Activity activity = activityReference.get();
        if(!Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))) return;

        try {
            OutputStream out_data = new FileOutputStream(new File(destDir, activity.getString(R.string.cf_dcmmask_name)));
            saveLargeImageToFile(out_data, JNIInterface.JNIgetVolumeData(true));
        } catch (Exception e) {
            Log.e(TAG, "====Failed to Save Masks to file");
        }
    }
    private void update_local_info(datasetInfo tds, volumeInfo tvol){
        String dsname = tds.getFolderName();

        if(!local_dv_map.containsKey(dsname)){
            available_local_datasets.add(tds);
            local_dv_map.put(dsname, new ArrayList<>());
        }
        List<volumeInfo> infolist = local_dv_map.get(dsname);
        infolist.add(tvol);
        local_dv_map.put(dsname, infolist);
    }
    private void saveDCMIData(){
        Activity activity = activityReference.get();

//        //check if it's in local already
//        String dsname = target_ds.getFolderName();
//        if(local_dv_map.containsKey(dsname) && local_dv_map.get(dsname).contains(target_vol)) {        finished = true; return;}

        //update local data
        update_local_info(target_ds, target_vol);

        //save to local file
        if(Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))){
            try{
                String content = target_ds.getPatientName()+","
                        +target_ds.getDate()+","
                        +target_ds.getFolderName()+","
                        +target_vol.getFolderName()+","
                        +target_vol.getImgWidth()+","
                        +target_vol.getImgHeight()+","
                        +target_vol.getFileNums();
                fileUtils.addToFile(local_index_filename, content);

                //save data
                File datafile = Paths.get(target_root_dir, target_ds.getFolderName(), target_vol.getFolderName(),  activity.getString(R.string.cf_dcm_name)).toFile();
                saveLargeImageToFile(new FileOutputStream(datafile), JNIInterface.JNIgetVolumeData(false));
            }catch (Exception e){
                Log.e(TAG, "====Failed to Save Results to file");
            }
        }
        finished = true;
    }

    private boolean LoadDataFromLocal(String vpath){
        Activity activity = activityReference.get();
        //target folder exist
            File destDir = Paths.get(target_root_dir, vpath).toFile();
            if(!destDir.exists()) return false;
            //load data
            try{
                loadVolumeData(new FileInputStream(new File(destDir, activity.getString(R.string.cf_dcm_name))), false);
            }catch(Exception e){
                e.printStackTrace();
                return false;
            }

            //load mask
            try{
                loadVolumeData(new FileInputStream(new File(destDir, activity.getString(R.string.cf_dcmmask_name))), true);
            }catch(Exception e){
                Log.e(TAG, "===LoadCachedData: no masks found" );
            }
            return true;
    }

    private void loadVolumeData(InputStream instream, boolean isMask)
        throws IOException{
            byte[] chunk = new byte[1024];
            int id = 0;
            int len;
            if(isMask){
                while ((len = instream.read(chunk)) != -1) {
//                    JNIInterface.JNIsendDCMIMask(id, len, chunk);
                    JNIInterface.JNIsendData(1, id, len, 2, chunk);
                    id++;
                }
                finished_mask = true;
            }
            else{
                while ((len = instream.read(chunk)) != -1) {
//                    JNIInterface.JNIsendDCMImg(id, len, chunk);
                    JNIInterface.JNIsendData(0, id, len, 2, chunk);
                    id++;
                }
                finished = true;
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

}

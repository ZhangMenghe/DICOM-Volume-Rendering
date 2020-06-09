package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import helmsley.vr.DUIs.DSCardRecyclerViewAdapter;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.Utils.fileUtils;
import helmsley.vr.dicomManager;
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

import com.google.common.primitives.Booleans;
import com.google.common.primitives.Ints;
import com.google.protobuf.ByteString;

public class fileTransferClient {
    final static String TAG = "fileTransferClient";
    private static WeakReference<fileTransferClient> selfReference;
    private static boolean finished = false, finished_mask=false;

    private ManagedChannel mChannel;

    private final int CLIENT_ID = 1;
    private datasetInfo target_ds;
    private volumeInfo target_vol;
    private final WeakReference<Activity> activityReference;

    private List<datasetInfo> available_remote_datasets,
            available_local_datasets = new ArrayList<>();
    private List<configResponse.configInfo> available_config_files;
    private Map<String, List<volumeInfo>> local_dv_map = new HashMap<>();

    private boolean local_initialized = false;
    private boolean config_dirty = true;
    private static String DCM_FILE_NAME, DCM_MASK_FILE_NAME, DCM_WMASK_FILE_NAME;
    private static String TARGET_ROOT_DIR, LOCAL_INDEX_FILE_PATH;
    public fileTransferClient(Activity activity){
        activityReference = new WeakReference<Activity>(activity);
        TARGET_ROOT_DIR = activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name);
        selfReference = new WeakReference<>(this);
        LOCAL_INDEX_FILE_PATH = TARGET_ROOT_DIR + "/" + activityReference.get().getString(R.string.cf_config_name);
        DCM_FILE_NAME = activity.getString(R.string.cf_dcm_name);
        DCM_MASK_FILE_NAME = activity.getString(R.string.cf_dcmmask_name);
        DCM_WMASK_FILE_NAME = activity.getString(R.string.cf_dcmwmask_name);
    }
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            Request req = Request.newBuilder().setClientId(CLIENT_ID).build();
            dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
            available_remote_datasets =  blockingStub.getAvailableDatasets(req).getDatasetsList();
            available_config_files = blockingStub.getAvailableConfigs(req).getConfigsList();
            config_dirty = false;
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
        List<String> lpc = fileUtils.readLines(LOCAL_INDEX_FILE_PATH);
        int record_num = lpc.size() / 2;
        if(lpc.isEmpty()){
            Log.i(TAG, "=== SetupLocal: No existing index file exist====");
            return;
        }
        for(int i=0; i<record_num; i++){
//            Log.e(TAG, "===SetupLocal: read " + line);
            String[] info = lpc.get(i*2).split("#");
            //Larry Smarr/2016-10-26/Larry_Smarr_2016/series_23_Cor_LAVA_PRE-Amira/series_23_Cor_LAVA_PRE-Amira/512, 512, 144/1.0, -0.0, 0.0, -0.0, -0.0, -1.0/0.8984, 0.8984/243.10002131

            datasetInfo tinfo = datasetInfo.newBuilder().setPatientName(info[0]).setDate(info[1]).setFolderName(info[2]).build();

            volumeInfo.Builder vinfo_builder = volumeInfo.newBuilder()
                    .setFolderName(info[3])
                    .setFolderPath(info[4])
                    .setVolumeLocRange(Float.parseFloat(info[8]))
                    .setWithMask(Boolean.parseBoolean(info[9]))
                    .setDataSourceValue(Integer.parseInt(info[10]));
            //set dimensions
            String[] dims_tx = info[5].split(",");
            for(String dt:dims_tx)
                vinfo_builder.addDims(Integer.parseInt(dt));
            //set orientation
            String[] ori_tx = info[6].split(",");
            for(String t:ori_tx)
                vinfo_builder.addOrientation(Float.parseFloat(t));
            //set resolution
            String[] res_tx = info[7].split(",");
            for(String t:res_tx)
                vinfo_builder.addResolution(Float.parseFloat(t));

            try{
                File data = new File(TARGET_ROOT_DIR, info[2] + '/' + info[3] + "/sample");
                InputStream inputStream = new FileInputStream(data);
                byte[] buffer = new byte[inputStream.available()];
                inputStream.read(buffer);
                ByteString simg = ByteString.copyFrom(buffer);
                vinfo_builder.setSampleImg(simg);
            }catch (Exception e) {
                Log.e(TAG, "===Failed to open sample image "+ e);
            }
//           group/rank/rscore/...../volscore*3
            String[] score_tx = lpc.get(i*2+1).split("#");
            volumeResponse.scoreInfo.Builder s_builder = volumeResponse.scoreInfo.newBuilder()
                    .setRgroupId(Integer.parseInt(score_tx[0]))
                    .setRankId(Integer.parseInt(score_tx[1]))
                    .setRankScore(Float.parseFloat(score_tx[2]));
            int param_end_id = score_tx.length - 3;
            for(int ri=3; ri<param_end_id; ri++)
                s_builder.addRawScore(Float.parseFloat(score_tx[ri]));
            for(int ni=param_end_id; ni<score_tx.length; ni++)
                s_builder.addVolScore(Float.parseFloat(score_tx[ni]));

            vinfo_builder.setScores(s_builder.build());
            update_local_info(tinfo, vinfo_builder.build());
        }
        local_initialized = true;
    }
    public List<configResponse.configInfo> getAvailableConfigFiles(){
        if(config_dirty){
            try{
                Request req = Request.newBuilder().setClientId(CLIENT_ID).build();
                dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
                available_config_files = blockingStub.getAvailableConfigs(req).getConfigsList();
                config_dirty = false;
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
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
        Iterator<volumeResponse> data_itor;

        List<volumeInfo> res_lst = new ArrayList<>();
        data_itor = blockingStub.getVolumeFromDataset(req);
        while(data_itor.hasNext())
            res_lst.addAll(data_itor.next().getVolumesList());
        return res_lst;
    }
    public boolean deleteLocalData(String dsname, volumeInfo rinfo){
        //delete from local list
//        if(!local_dv_map.containsKey(dsname)) return false;
        List<volumeInfo> infolist = local_dv_map.get(dsname);
//        volumeInfo rinfo = infolist.get(pos);
        if(rinfo == null) return false;

        //remove data directory
        File tar_ds_dir = Paths.get(TARGET_ROOT_DIR, dsname).toFile();
        if(!tar_ds_dir.exists()) return false;
        if(!fileUtils.deleteDirectory(new File(tar_ds_dir, rinfo.getFolderName()))) return false;

        //remove from local index file
        //todo: flush if many changes, avoid write line by line
        List<String> lines = fileUtils.readLines(LOCAL_INDEX_FILE_PATH);
        int rnum = (int)lines.size() / 2;
        for(int i=0; i<rnum; i++){
            String[] info = lines.get(2*i).split("#");
            if(info[2].equals(dsname)
                    && info[3].equals(rinfo.getFolderName())){
                lines.remove(lines.get(2*i+1));
                lines.remove(lines.get(2*i));
                fileUtils.writeToFile(LOCAL_INDEX_FILE_PATH, lines);
                infolist.remove(rinfo);

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

    public void ExportConfig(String content){
        if(content == null) return;
        Request req = Request.newBuilder().setClientId(CLIENT_ID).setReqMsg(content).build();
        dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
        Response res = blockingStub.exportConfigs(req);
        if(res.getSuccess()) Toast.makeText(activityReference.get(), "Config Exported", Toast.LENGTH_LONG).show();
        config_dirty = true;
    }
    public boolean Download(String ds_name, volumeInfo target_volume){
        target_vol = target_volume;
        if(target_volume.getDataSource() == volumeInfo.DataSource.DEVICE){
            if(dicomManager.LoadDataFromDevice(target_volume)) return true;
            Toast.makeText(activityReference.get(),  "File not exist, please check device", Toast.LENGTH_LONG).show();
            return false;
        }else{
            //todo: check and timeout for loading failure
            if(!LoadDataFromLocal(ds_name + "/" + target_vol.getFolderName()))
                new GrpcTask(new DownloadDICOMRunnable(), mChannel, this).execute(Paths.get(target_ds.getFolderName(), target_volume.getFolderName()).toString());
        }
        return true;
    }
    private void DownloadMasks(String target_path){
        if(!target_vol.getWithMask()) return;
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
            fileTransferClient.saveDCMI(activity.target_ds, activity.target_vol, true);
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

    private static File get_tar_vol_dir(String root_dir, String ds_name, String vol_name){
        File tar_ds_dir = Paths.get(root_dir, ds_name).toFile();
        if(!tar_ds_dir.exists()) tar_ds_dir.mkdir();

        //check relevant sub dirs
        String[] vol_dir_tree = vol_name.split("/");
        String vol_dir_prefix="";
        for(String vol_dir:vol_dir_tree){
            vol_dir_prefix = vol_dir_prefix+vol_dir;
            File tar_vol_dir = new File(tar_ds_dir, vol_dir_prefix);
            if(!tar_vol_dir.exists()) tar_vol_dir.mkdir();
        }
        File tar_vol_dir = new File(tar_ds_dir, vol_name);
        if(!tar_vol_dir.exists()) tar_vol_dir.mkdir();
        return tar_vol_dir;
    }

    //save after download complete
    private void SaveMasks(){
        dialogUIs.FinishMaskLoading();
        Activity activity = activityReference.get();
        if(!Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))) return;

        try {
            File dataf = new File(get_tar_vol_dir(TARGET_ROOT_DIR, target_ds.getFolderName(), target_vol.getFolderName()), DCM_MASK_FILE_NAME);
            fileUtils.saveLargeImageToFile(new FileOutputStream(dataf), JNIInterface.JNIgetVolumeData());
        } catch (Exception e) {
            Log.e(TAG, "====Failed to Save Masks to file");
        }
    }
    //save after download complete
    public static void saveDCMI(datasetInfo tds, volumeInfo tvol, boolean save_complete){
        //update local data
        selfReference.get().update_local_info(tds, tvol);
        dialogUIs.local_dirty = true;

        try{
            String[] title_info = {tds.getPatientName(), tds.getDate(), tds.getFolderName(),tvol.getFolderName(), tvol.getFolderPath()};
            List<String>vol_info_lst = new ArrayList<>();
            vol_info_lst.add(String.join("#", title_info));
            //dims
            String listString = tvol.getDimsList().toString().replaceAll("\\s+","");
            vol_info_lst.add(listString.substring(1, listString.length()-1));
            //set orientation
            listString = tvol.getOrientationList().toString().replaceAll("\\s+","");
            vol_info_lst.add(listString.substring(1, listString.length()-1));
            //set resolution
            listString = tvol.getResolutionList().toString().replaceAll("\\s+","");
            vol_info_lst.add(listString.substring(1, listString.length()-1));
            //set loc range
            vol_info_lst.add(String.valueOf(tvol.getVolumeLocRange()));
            //mask
            vol_info_lst.add(String.valueOf(tvol.getWithMask()));
            //datasource
            vol_info_lst.add(String.valueOf(tvol.getDataSourceValue()));
            //set score
            volumeResponse.scoreInfo sinfo = tvol.getScores();
//                String[] score_info = {String.valueOf(sinfo.getRgroupId()), String.valueOf(sinfo.getRankScore()), String.valueOf(sinfo.getVolScore(0)), String.valueOf(sinfo.getVolScore(1)), String.valueOf(sinfo.getVolScore(2))};
            List<String> score_info_lst = new ArrayList<>();
            score_info_lst.add(String.valueOf(sinfo.getRgroupId()));
            score_info_lst.add(String.valueOf(sinfo.getRankId()));
            score_info_lst.add(String.valueOf(sinfo.getRankScore()));
            for(Float s:sinfo.getRawScoreList())
                score_info_lst.add(String.valueOf(s));
            for(Float s:sinfo.getVolScoreList())
                score_info_lst.add(String.valueOf(s));

            String content = String.join("#", vol_info_lst) + '\n' + String.join("#", score_info_lst) + '\n';
            fileUtils.addToFile(LOCAL_INDEX_FILE_PATH, content);
            //save sample file
            byte[]buffer = tvol.getSampleImg().toByteArray();
            File simgf = new File(get_tar_vol_dir(TARGET_ROOT_DIR, tds.getFolderName(), tvol.getFolderName() ), "sample");
            FileOutputStream os = new FileOutputStream(simgf);
            os.write(buffer);
            if(save_complete){
                File dataf = new File(get_tar_vol_dir(TARGET_ROOT_DIR, tds.getFolderName(), tvol.getFolderName()), tvol.getWithMask()?DCM_WMASK_FILE_NAME:DCM_FILE_NAME);
                fileUtils.saveLargeImageToFile(new FileOutputStream(dataf), JNIInterface.JNIgetVolumeData());
            }

        }catch (Exception e){
            e.printStackTrace();
            Log.e(TAG, "====Failed to Save Results to file");
        }
        finished = true;
        if(!save_complete)DSCardRecyclerViewAdapter.DirtyCache(tds.getFolderName());
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
        Path destDir_path = Paths.get(TARGET_ROOT_DIR, vpath);
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

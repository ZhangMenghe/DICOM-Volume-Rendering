package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.stub.StreamObserver;
import helmsley.vr.proto.datasetResponse.datasetInfo;
import helmsley.vr.proto.volumeResponse.volumeInfo;
import java.io.*;
import java.lang.ref.WeakReference;
import java.nio.file.Paths;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.TimeUnit;


public class fileTransferClient {
    final static String TAG = "fileTransferClient";
    public static boolean finished = false;

    private ManagedChannel mChannel;

    public final int CLIENT_ID = 1;
    private String target_ds, target_vol;

    private final WeakReference<Activity> activityReference;

    public fileTransferClient(Activity activity){
        activityReference = new WeakReference<Activity>(activity);
    }
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            return "";
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
    }
    public List<datasetInfo> getAvailableDataset(){
        Request req = Request.newBuilder().setClientId(CLIENT_ID).build();
        dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
        return blockingStub.getAvailableDatasets(req).getDatasetsList();
    }

    public List<volumeInfo> requestVolumesFromDataset(String dataset_name){
        target_ds = dataset_name;
        Request req = Request.newBuilder().setClientId(CLIENT_ID).setReqMsg(target_ds).build();
        dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
        return blockingStub.getVolumeFromDataset(req).getVolumesList();
    }
    public void Download(String target_volume){
        new GrpcTask(new DownloadDICOMRunnable(), mChannel, this).execute(Paths.get(target_ds, target_volume).toString());
    }
    public void DownloadMasks(String target_volume){
        new GrpcTask(new DownloadMasksRunnable(), mChannel, this).execute(Paths.get(target_ds, target_volume).toString());
    }
//    public void SaveDatasetToFile(bundleConfig config_, String target_path, String folder_name, String config_name, String data_name)
//            throws IOException {
//        String targetLocation = target_path + "/"+folder_name;
//        File destDir = new File(targetLocation);
//        if(!destDir.exists()) destDir.mkdirs();
//
//        OutputStream out_config = new FileOutputStream(targetLocation + "/" + config_name);
//        OutputStreamWriter config_writer = new OutputStreamWriter(out_config);
//        //save config
//        String content = config_.getFolderName() + "\n"
//                +config_.getFileNums() + "\n"
//                +config_.getImgHeight() + "\n"
//                +config_.getImgWidth();
//        config_writer.write(content);
//        config_writer.close();
//        out_config.close();
//
//        String targetLocation_img = targetLocation + "/" + data_name;
//        OutputStream out_img = new FileOutputStream(targetLocation_img);
//        out_img.write(JNIInterface.JNIgetVolumeData(false));
//        out_img.close();
//    }

    public void SaveImagesToFile(String target_path, String folder_name, String data_name, boolean b_mask)throws IOException {
        String targetLocation = target_path + "/"+folder_name;
        String targetLocation_img = targetLocation + "/" + data_name;
        OutputStream out_img = new FileOutputStream(targetLocation_img);
        out_img.write(JNIInterface.JNIgetVolumeData(b_mask));
        out_img.close();
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
        @Override
        public String run(String folder_path, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub)
                throws Exception{
            Request req = Request.newBuilder().setClientId(1).setReqMsg(folder_path).build();
            Iterator<dcmImage> dcm_img_iterator;
            dcm_img_iterator = blockingStub.download(req);
            while(dcm_img_iterator.hasNext()){
                dcmImage img = dcm_img_iterator.next();
                //tackle with the image here
                Log.e(TAG, "====img:" + img.getPosition() );
                JNIInterface.JNIsendDCMImg(img.getDcmID(), img.getPosition(), img.getData().toByteArray());
            }
            return "===Success!\n";
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
            activity.saveResults();
            //todo

//            activity.DownloadMasks(folder_name_);
        }
    }
    private static class DownloadMasksRunnable implements GrpcRunnable{
        private String folder_name_;
        @Override
        public String run(String folder_name, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub)
                throws Exception{
            folder_name_ = folder_name;
            Request req = Request.newBuilder().setClientId(1).build();
            StreamObserver<dcmImage> mask_observer = new StreamObserver<dcmImage>() {
                @Override
                public void onNext(dcmImage value) {
//                    Log.e(TAG, "==========onNext: "+ value.getPosition() );
                    JNIInterface.JNIsendDCMIMask(value.getDcmID(), value.getPosition(), value.getData().toByteArray());
                }

                @Override
                public void onError(Throwable t) {

                }

                @Override
                public void onCompleted() {
                    Log.i(TAG, "==============Finish Loading Masks========= " );
                }
            };
            asyncStub.downloadMasks(req, mask_observer);
            return "success";
        }
        @Override
        public void onPostExecute(fileTransferClient activity){
            activity.SaveMasks(folder_name_);
        }
    }
    private void SaveMasks(String folder_name){
        Activity activity = activityReference.get();
        if(Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))){
            try{
                SaveImagesToFile(
                        activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name),
                        folder_name,
                        activity.getString(R.string.cf_dcmmask_name),
                        true);
            }catch (Exception e){
                Log.e(TAG, "====Failed to Save Masks to file");
            }
        }
    }
    private void saveResults(){
        //todo: save results

//        Activity activity = activityReference.get();
//        if(Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))){
//            try{
//                SaveDatasetToFile(
//                        config,
//                        activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name),
//                        folder_name,
//                        activity.getString(R.string.cf_config_name),
//                        activity.getString(R.string.cf_dcmfolder_name));
//            }catch (Exception e){
//                Log.e(TAG, "====Failed to Save Results to file");
//            }
//        }
        finished = true;
    }

    public void Shutdown() throws InterruptedException {
        mChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }

}

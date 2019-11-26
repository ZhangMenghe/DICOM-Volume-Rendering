package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import java.io.*;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;


public class fileTransferClient {
    final static String TAG = "fileTransferClient";
    public static boolean finished = false;

    private ManagedChannel mChannel;
//    private final dataTransferGrpc.dataTransferStub mAsyncStub;
//    private final fileChunkerGrpc.fileChunkerBlockingStub mBlockStub;

    public final int CLIENT_ID = 1;
    private ArrayList<datasetInfo> data_info_lst;

    private final WeakReference<Activity> activityReference;

    public fileTransferClient(Activity activity){
        activityReference = new WeakReference<Activity>(activity);
    }
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            data_info_lst = getAvailableDatasetInfos();
            return "";
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
    }
    public ArrayList<datasetInfo> getAvailableDataset(){
        return data_info_lst;
    }
    private ArrayList<datasetInfo> getAvailableDatasetInfos(){
        ArrayList<datasetInfo> data_arr = new ArrayList<>();
        Request req = Request.newBuilder().setClientId(1).build();
        dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(mChannel);
        Iterator<datasetInfo> info_itr = blockingStub.getAvailableDatasetInfos(req);
        while(info_itr.hasNext())
            data_arr.add(info_itr.next());
        for(datasetInfo info:data_arr)
            Log.e(TAG, "getAvailableDatasetInfos: " + info.getFolderName() );
        return data_arr;
    }
    public void Download(String folder_name){new GrpcTask(mChannel, this).execute(folder_name);}
    public void SaveDatasetToFile(bundleConfig config_, String target_path, String folder_name, String config_name, String data_name)
            throws IOException {
        String targetLocation = target_path + "/"+folder_name;
        File destDir = new File(targetLocation);
        if(!destDir.exists()) destDir.mkdirs();

        OutputStream out_config = new FileOutputStream(targetLocation + "/" + config_name);
        OutputStreamWriter config_writer = new OutputStreamWriter(out_config);
        //save config
            String content = config_.getFolderName() + "\n"
                    +config_.getFileNums() + "\n"
                    +config_.getImgHeight() + "\n"
                    +config_.getImgWidth();
            config_writer.write(content);
        config_writer.close();
        out_config.close();

        String targetLocation_img = targetLocation + "/" + data_name;
        OutputStream out_img = new FileOutputStream(targetLocation_img);
        out_img.write(JNIInterface.JNIgetVolumeData());
        out_img.close();
    }

    private static class GrpcTask extends AsyncTask<String, Void, String> {
        private final ManagedChannel channel;
        private final WeakReference<fileTransferClient> activityReference;
        private bundleConfig dataset_config;
        private String folder_name;

        GrpcTask(ManagedChannel channel, fileTransferClient activity) {
            this.channel = channel;
            this.activityReference = new WeakReference<fileTransferClient>(activity);
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                folder_name = params[0];
                Request req = Request.newBuilder().setClientId(1).setReqMsg(folder_name).build();
                dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(channel);
//                dataTransferGrpc.dataTransferStub asyncStub = dataTransferGrpc.newStub(channel);
//                bundleConfig config = grpcRunnable.run(req, dataTransferGrpc.newBlockingStub(channel), dataTransferGrpc.newStub(channel));
                dataset_config =  blockingStub.getConfig(req);
                Log.e(TAG, "===returned configs: " + dataset_config.getFolderName() + "nums:" + dataset_config.getFileNums());

                JNIInterface.JNIsetupDCMIConfig(dataset_config.getImgWidth(), dataset_config.getImgHeight(), dataset_config.getFileNums());

                Iterator<dcmImage> dcm_img_iterator;
                dcm_img_iterator = blockingStub.download(req);
                while(dcm_img_iterator.hasNext()){
                    dcmImage img = dcm_img_iterator.next();
                    //tackle with the image here
                    Log.e(TAG, "====img:" + img.getPosition() );
                    JNIInterface.JNIsendDCMImg(img.getDcmID(), img.getPosition(), img.getData().toByteArray());
                }
                return "===Success!\n";
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
//            try {
//                channel.shutdown().awaitTermination(1, TimeUnit.SECONDS);
//            } catch (InterruptedException e) {
//                Thread.currentThread().interrupt();
//            }
            fileTransferClient activity = activityReference.get();
            if (activity == null) {
                return;
            }
            activity.saveResults(folder_name, dataset_config);

        }
    }

    private void saveResults(String folder_name, bundleConfig config){
        Activity activity = activityReference.get();
        if(Boolean.parseBoolean(activity.getString(R.string.cf_b_cache))){
            try{
                SaveDatasetToFile(
                        config,
                        activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name),
                        folder_name,
                        activity.getString(R.string.cf_config_name),
                        activity.getString(R.string.cf_dcmfolder_name));
            }catch (Exception e){
                Log.e(TAG, "====Failed to Save Results to file");
            }
        }
        finished = true;
    }

    public void Shutdown() throws InterruptedException {
        mChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }

}

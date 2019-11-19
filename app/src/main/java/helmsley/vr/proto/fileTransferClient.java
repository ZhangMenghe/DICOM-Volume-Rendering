package helmsley.vr.proto;

import android.os.AsyncTask;
import android.util.Log;

import helmsley.vr.JNIInterface;
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
//    private ArrayList<byte[]> image_arrs;

    public fileTransferClient(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            Log.e(TAG, "===Failed... : " + sw);
            return;
        }
    }
    public fileTransferClient(){}
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            ArrayList<datasetInfo> data_info_lst = getAvailableDatasetInfos();
            return "";
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
    }

    public ArrayList<datasetInfo> getAvailableDatasetInfos(){
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
    public void Run(){new GrpcTask(mChannel, this).execute();}

    private static class GrpcTask extends AsyncTask<Void, Void, String> {
        private final ManagedChannel channel;
        private final WeakReference<fileTransferClient> activityReference;

        GrpcTask(ManagedChannel channel, fileTransferClient activity) {
            this.channel = channel;
            this.activityReference = new WeakReference<fileTransferClient>(activity);
        }

        @Override
        protected String doInBackground(Void... nothing) {
            try {
                Request req = Request.newBuilder().setClientId(1).setReqMsg("dicom_sample").build();
                dataTransferGrpc.dataTransferBlockingStub blockingStub = dataTransferGrpc.newBlockingStub(channel);
//                dataTransferGrpc.dataTransferStub asyncStub = dataTransferGrpc.newStub(channel);
//                bundleConfig config = grpcRunnable.run(req, dataTransferGrpc.newBlockingStub(channel), dataTransferGrpc.newStub(channel));
                bundleConfig vconfig =  blockingStub.getConfig(req);
                Log.e(TAG, "===returned configs: " + vconfig.getFolderName() + "nums:" + vconfig.getFileNums());

                JNIInterface.JNIsetupDCMIConfig(vconfig.getImgWidth(), vconfig.getImgHeight(), vconfig.getFileNums());

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
            activity.showResults(result);

        }
    }

    private void showResults(String txt){
        //do nothing
        finished = true;

    }

    public void Shutdown() throws InterruptedException {
        mChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }
}

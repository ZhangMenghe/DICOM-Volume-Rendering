package helmsley.vr.proto;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import com.google.protobuf.ByteString;

import helmsley.vr.R;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import io.grpc.stub.StreamObserver;

import java.io.*;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Spliterator;
import java.util.Spliterators;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;

import helmsley.vr.proto.dataTransferGrpc;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class fileTransferClient {
    final static String TAG = "fileTransferClient";

    private ManagedChannel mChannel;
//    private final dataTransferGrpc.dataTransferStub mAsyncStub;
//    private final fileChunkerGrpc.fileChunkerBlockingStub mBlockStub;

    public final int CLIENT_ID = 1;
//    private ArrayList<byte[]> image_arrs;
    private GrpcTask tsk;

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

//        tsk = new GrpcTask(mChannel, this);
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

                Iterator<dcmImage> dcm_img_iterator;
                dcm_img_iterator = blockingStub.download(req);
                while(dcm_img_iterator.hasNext()){
                    dcmImage img = dcm_img_iterator.next();
                    //tackle with the image here
                    Log.e(TAG, "====img:" + img.getPosition() );
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
    }

    public void Shutdown() throws InterruptedException {
        mChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }
}

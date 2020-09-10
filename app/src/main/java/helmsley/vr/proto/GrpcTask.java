package helmsley.vr.proto;

import android.os.AsyncTask;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.ref.WeakReference;

import io.grpc.ManagedChannel;

public class GrpcTask extends AsyncTask<String, Void, String> {
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
    interface GrpcRunnable {
        /** Perform a grpcRunnable and return all the logs. */
        String run(String folder_name, dataTransferGrpc.dataTransferBlockingStub blockingStub, dataTransferGrpc.dataTransferStub asyncStub) throws Exception;
        void onPostExecute(fileTransferClient activity);
    }
}

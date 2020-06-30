package helmsley.vr.proto;

import android.app.Activity;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.ref.WeakReference;

import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.JNIInterface;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class rpcManager {
    final static String TAG = "rpcManager";
    public final static int CLIENT_ID = 1;

    private final WeakReference<Activity> actRef;
    private final WeakReference<dialogUIs> duiRef;

    public static ManagedChannel mChannel;
    public static dataTransferGrpc.dataTransferBlockingStub data_stub;
    public static dataTransferGrpc.dataTransferStub mask_stub;
    public static inspectorSyncGrpc.inspectorSyncStub operate_stub;

    private fileTransferClient data_manager;
    private operateClient opa_manager;

    public rpcManager(Activity activity, dialogUIs dui) {
        actRef = new WeakReference<Activity>(activity);
        duiRef = new WeakReference<>(dui);
        data_manager = new fileTransferClient(activity, dui);

    }

    public String Setup(String host, String portStr) {
        try {
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            data_stub = dataTransferGrpc.newBlockingStub(mChannel);
            mask_stub = dataTransferGrpc.newStub(mChannel);
            operate_stub = inspectorSyncGrpc.newStub(mChannel);
            data_manager.Setup();
        } catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
        return "";
    }

    public void SetupLocal(){
        data_manager.SetupLocal();
    }

    public boolean CheckProcessFinished(){
        if(data_manager.isDownloadingProcessFinished()){
            data_manager.Reset();
            JNIInterface.JNIsendDataDone();
            return true;
        }
        if(data_manager.isDownloadingMaskProcessFinished()){
            data_manager.ResetMast();
            JNIInterface.JNIsendDataDone();
        }
        return false;
    }
    public fileTransferClient getDataManager(){return data_manager;}
}

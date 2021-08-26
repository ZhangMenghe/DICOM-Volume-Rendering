package helmsley.vr.proto;

import android.app.Activity;
import android.util.Log;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

import helmsley.vr.DUIs.JUIInterface;
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

    private fileTransferClient data_manager;
    private operateClient opa_manager;
    private boolean isSetup = false;

    public rpcManager(Activity activity, dialogUIs dui) {
        actRef = new WeakReference<Activity>(activity);
        duiRef = new WeakReference<>(dui);
        data_manager = new fileTransferClient(activity, dui);
        opa_manager = new operateClient();
    }
    public String getTargetDatasetName(){return data_manager.getTargetDatasetName();}
    public String getTargetVolumeName(){return data_manager.getTargetVolumeName();}
    public String Setup(String host, String portStr) {
        try {
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            data_stub = dataTransferGrpc.newBlockingStub(mChannel);
            mask_stub = dataTransferGrpc.newStub(mChannel);
            opa_manager.Setup(mChannel);
            data_manager.Setup();
            isSetup = true;
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

    public void CheckDataLoading(){
        if(data_manager.isDownloadingProcessFinished()){
            data_manager.Reset();
            JNIInterface.JNIsendDataDone();
        }
        if(data_manager.isDownloadingMaskProcessFinished()){
            data_manager.ResetMast();
            JNIInterface.JNIsendDataDone();
        }
        if(data_manager.isDownloadingCenterlineFinished()){
            data_manager.ResetCenterline();
            JNIInterface.JNIsendDataDone();
        }

    }

    public void UpdateVolumePose(){
        if(!isSetup || !JUIInterface.on_broadcast)
        {
            return;
        }

        if(JUIInterface.fingerCount > 0){

            // Check if change and get pose info
            float[] pos = new float[3];
            float[] rot = new float[4];
            float[] scale = new float[1];
            boolean flag = JUIInterface.JUIgetCurrentPose(pos, rot,scale);
            if(flag){
                JUIInterface.JUIonChangePose(pos, rot, scale[0]);
            }
        }else {

            float[] pos = new float[3];
            float[] rot = new float[4];
            float[] scale = new float[1];
            boolean flag = JUIInterface.JUIgetCurrentPose(pos, rot,scale);
            //if(!flag) {
                List<Float> res_pos = operateClient.gsVolumePose(ReqType.GET, VPMsg.VPType.POS, pos);
                List<Float> res_rot = operateClient.gsVolumePose(ReqType.GET, VPMsg.VPType.ROT, rot);
                List<Float> res_scale = operateClient.gsVolumePose(ReqType.GET, VPMsg.VPType.SCALE, scale);
                // currently update volume pose here

                pos[0] = res_pos.get(0);
                pos[1] = res_pos.get(1);
                pos[2] = res_pos.get(2);

                rot[0] = res_rot.get(0);
                rot[1] = res_rot.get(1);
                rot[2] = res_rot.get(2);
                rot[3] = res_rot.get(3);

                scale[0] = res_scale.get(0);

                JUIInterface.JUIsetCurrentPose(pos, rot, scale);

                //Log.d("RPC", "update:" + scale[0]);
            //}
        }

    }
    public fileTransferClient getDataManager(){return data_manager;}
}

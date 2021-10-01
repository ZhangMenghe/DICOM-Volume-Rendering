package helmsley.vr.proto;

import android.app.Activity;
import android.view.View;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.ref.WeakReference;
import java.util.List;

import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.JNIInterface;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class rpcManager {
    final static String TAG = "rpcManager";
    public final static int CLIENT_ID = 1;
    public static boolean G_JOIN_SYNC = false;
    public static boolean G_STATUS_SENDER = false;
    public static boolean G_FORCED_STOP_BROADCAST = false;

    private final WeakReference<Activity> actRef;
    private final WeakReference<dialogUIs> duiRef;

    public static ManagedChannel mChannel;
    public static dataTransferGrpc.dataTransferBlockingStub data_stub;
    public static dataTransferGrpc.dataTransferStub mask_stub;

    private fileTransferClient data_manager;
    private operateClient opa_manager;

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

    public void updateOnFrame() {
        check_data_loading();
        if(G_FORCED_STOP_BROADCAST){
            duiRef.get().onBroadCastStatusChanged(false);
            G_FORCED_STOP_BROADCAST = false;
        }
        if (G_JOIN_SYNC) {
            if (G_STATUS_SENDER) check_server_status();
            else update_volume_pose();
        }
    }
    public void changeSyncStatus(boolean join, boolean broadcast){
        G_JOIN_SYNC = join;
        G_STATUS_SENDER = broadcast;

        if(join){
            if(broadcast) opa_manager.startBroadcast();
            else opa_manager.joinAsReceiver();
        }else{
            opa_manager.ExitSync();
        }
    }
    private void check_data_loading(){
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
    private void check_server_status(){
        opa_manager.checkCurrentBroadcaster();
    }
    private void update_volume_pose(){
        List<VPMsg> msg_lst = opa_manager.getPoseUpdates();
        if(msg_lst.isEmpty()){//Check for operations
            List<GestureOp> op_lst = opa_manager.getOperations();
            if(!op_lst.isEmpty()){
                for(GestureOp op:op_lst){
                    GestureOp.OPType type = op.getType();
                    if(type == GestureOp.OPType.TOUCH_DOWN)JUIInterface.JUIonSingleTouchDownNative(0, op.getX(), op.getY());
                    else if(type == GestureOp.OPType.TOUCH_MOVE)JUIInterface.JUIonTouchMoveNative(op.getX(), op.getY());
                    else if(type == GestureOp.OPType.TOUCH_UP)JUIInterface.JUIonSingleTouchUpNative();
                }
            }
        }else{//Check for pose operations
            boolean[]volume_pose_type = new boolean[]{false, false, false};
            float[]volume_pose = new float[10];

            for(VPMsg msg:msg_lst){
                if(msg.getVolumePoseType() == VPMsg.VPType.ROT){
                    //rotation:(w,x,y,z)
                    for(int i=0; i<4; i++) volume_pose[i]= msg.getValues(i);
                    volume_pose_type[0] = true;
                }else if(msg.getVolumePoseType() == VPMsg.VPType.SCALE){
                    for(int i=0; i<3; i++) volume_pose[4+i]= msg.getValues(i);
                    volume_pose_type[1] = true;
                }else{
                    for(int i=0; i<3; i++) volume_pose[7+i]= msg.getValues(i);
                    volume_pose_type[2] = true;
                }
            }
            JUIInterface.JUIsetVolumePose(volume_pose_type, volume_pose);
        }
    }
    public fileTransferClient getDataManager(){return data_manager;}
}

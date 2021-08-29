package helmsley.vr.proto;

import android.util.Log;

import com.google.common.primitives.Booleans;
import com.google.common.primitives.Floats;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.DUIs.BasePanel;
import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.JNIInterface;
import io.grpc.ManagedChannel;
import io.grpc.stub.StreamObserver;

public class operateClient {
    final static String TAG = "operateClient";

    private static inspectorSyncGrpc.inspectorSyncStub operate_stub;
    private static StreamObserver<commonResponse> observer;

    private static Request.Builder request_builder;
    private static GestureOp.Builder gesture_builder;
    private static ResetMsg.Builder reset_builder;
    private static TuneMsg.Builder tune_builder;
    private static CheckMsg.Builder check_builder;
    private static MaskMsg.Builder msk_builder;
    private static DataMsg.Builder data_builder;

    private static boolean initialized = false;
    operateClient(){
        request_builder = Request.newBuilder();
        gesture_builder = GestureOp.newBuilder();
        reset_builder = ResetMsg.newBuilder();
        tune_builder = TuneMsg.newBuilder();
        check_builder = CheckMsg.newBuilder();
        msk_builder = MaskMsg.newBuilder();
        data_builder = DataMsg.newBuilder();
        observer = new StreamObserver<commonResponse>() {
            @Override
            public void onNext(commonResponse value) {
            }

            @Override
            public void onError(Throwable t) {
            }

            @Override
            public void onCompleted() {
            }
        };
    }
    void Setup(ManagedChannel channel){
        operate_stub = inspectorSyncGrpc.newStub(channel);
        initialized = true;
    }
    public static void startBroadcast(){
        Request req = request_builder.setClientId(rpcManager.CLIENT_ID).build();
        operate_stub.startBroadcast(req, observer);
    }
    public static void requestReset( String[] check_keys, boolean[] check_value, float[] volume_pose, float[] camera_pose){
        if(!initialized) return;
        reset_builder.clear();
        reset_builder.setClientId(rpcManager.CLIENT_ID);
        for(String ck:check_keys)reset_builder.addCheckKeys(ck);
        for(boolean cv:check_value)reset_builder.addCheckValues(cv);
        for(float vp:volume_pose)reset_builder.addVolumePose(vp);
        for(float cp:camera_pose)reset_builder.addCameraPose(cp);
        operate_stub.requestReset(reset_builder.build(), observer);
    }
    public static void setGestureOp(GestureOp.OPType type, float x, float y){
        if(!initialized) return;
        gesture_builder.clear();
        GestureOp req = gesture_builder.setClientId(rpcManager.CLIENT_ID).setGid(System.currentTimeMillis()).setType(type).setX(x).setY(y).build();
        operate_stub.setGestureOp(req, observer);
//        ops++;
//        if(System.currentTimeMillis() - last_timestamp > BACH_TIME_MILLS){
//            ops = 0;
//            last_timestamp = System.currentTimeMillis();
//        }
    }
    public static void setCheckParams(String key, boolean value){
        if(!initialized) return;
        check_builder.clear();
        CheckMsg cm = check_builder.setClientId(rpcManager.CLIENT_ID).setKey(key).setValue(value).build();
//        if(rpcManager.operate_stub == null)
//            check_pool.add(cm);
//        else
            operate_stub.setCheckParams(cm, observer);
    }
    public static void setMaskParams(int num, int mbits){
        if(!initialized) return;
        msk_builder.clear();
        operate_stub.setMaskParams(msk_builder.setClientId(rpcManager.CLIENT_ID).setNum(num).setMbits(mbits).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, float[] values){
        if(!initialized) return;
        tune_builder.clear();

        for(float vp:values)tune_builder.addValues(vp);
        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int v){
        if(!initialized) return;
        tune_builder.clear();

        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(v).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int tar, float v){
        if(!initialized) return;
        tune_builder.clear();

        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(tar).setValue(v).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type){
        if(!initialized) return;
        tune_builder.clear();

        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int tar, boolean v){
        if(!initialized) return;
        tune_builder.clear();

        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(tar).setValue(v?1:0).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int tar, float[] values){
        if(!initialized) return;
        tune_builder.clear();

        for(float vp:values)tune_builder.addValues(vp);
        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(tar).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int tar, int sub_tar){
        if(!initialized) return;
        tune_builder.clear();
        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(tar).setSubTarget(sub_tar).build(), observer);
    }
    public static void setTuneParams(TuneMsg.TuneType type, int tar, int sub_tar, float value){
        if(!initialized) return;
        tune_builder.clear();

        operate_stub.setTuneParams(tune_builder.setClientId(rpcManager.CLIENT_ID).setType(type).setTarget(tar).setSubTarget(sub_tar).setValue(value).build(), observer);
    }
    public static void sendVolume(String ds_name, String vl_name){
        data_builder.clear();
        data_builder.setDsName(ds_name);
        data_builder.setVolumeName(vl_name);
        if(JUIInterface.on_broadcast &&initialized){
            operate_stub.setDisplayVolume(data_builder.setClientId(rpcManager.CLIENT_ID).build(), observer);
            Log.e(TAG, "============sendVolume: 1===" );
        }
    }
}

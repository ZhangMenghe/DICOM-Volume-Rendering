package helmsley.vr.DUIs;

import java.util.LinkedHashMap;

import helmsley.vr.proto.GestureOp;
import helmsley.vr.proto.TuneMsg;
import helmsley.vr.proto.operateClient;

public class JUIInterface {
    private static boolean on_broadcast = false;
//    static void startBroadcast(LinkedHashMap map){
//        on_broadcast = true;
////        operateClient.syncAll(map);
//    }
//    static void stopBroadcast(){on_broadcast = false;}
    static void setBroadcast(boolean on){on_broadcast = on;}
    public static void JUIonReset(boolean update_local, int num, String[] check_keys, boolean[] check_value, float[] volume_pose, float[] camera_pose){
        if(update_local) JUIonResetNative(num, check_keys, check_value, volume_pose, camera_pose);
        if(on_broadcast) operateClient.reqestReset(check_keys, check_value, volume_pose, camera_pose);
    }
    static void JUIsetGraphRect(int id, int width, int height, int left, int top){
        JUIsetGraphRectNative(id, width, height, left, top);
    }

    static void JUIaddTuneParams(int[] nums, float[] values){
        JUIaddTuneParamsNative(nums, values);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.ADD_ONE, values);
    }
    static void JUIsetTuneWidgetById(int wid){
        JUIsetTuneWidgetByIdNative(wid);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.SET_TARGET, wid);
    }
    static void JUIremoveTuneWidgetById(int wid){
        JUIremoveTuneWidgetByIdNative(wid);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.REMOVE_ONE, wid);
    }
    static void JUIremoveAllTuneWidget(){
        JUIremoveAllTuneWidgetNative();
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.REMOTE_ALL);
    }
    static void JUIsetTuneWidgetVisibility(int wid, boolean visible){
        JUIsetTuneWidgetVisibilityNative(wid, visible);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.SET_VISIBLE, wid, visible);
    }
    static void JUIsetAllTuneParamById(int tid, float[] values){
        JUIsetAllTuneParamByIdNative(tid, values);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.SET_ALL, tid, values);
    }
    static void JUIsetTuneParamById(int tid, int pid, float value){
        JUIsetTuneParamByIdNative(tid, pid, value);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.SET_ONE, tid, pid, value);
    }
    public static void JUIsetDualParamById(int pid, float minValue, float maxValue){
        JUIsetDualParamByIdNative(pid, minValue,maxValue);
    }

    static void JUIsetChecks(String key, boolean value){
        JUIsetChecksNative(key, value);
        if(on_broadcast) operateClient.setCheckParams(key, value);
    }

    public static float[] JUIgetVCStates(){
        return JUIgetVCStatesNative();
    }

    static void JUIsetCuttingPlane(int id, float value){
        JUIsetCuttingPlaneNative(id, value);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.CUT_PLANE, id, value);
    }

    static float[] JUIgetCuttingPlaneStatus(){
        return JUIgetCuttingPlaneStatusNative();
    }

    static void JUIsetMaskBits(int num, int mbits){
        JUIsetMaskBitsNative(num, mbits);
        if(on_broadcast) operateClient.setMaskParams(num, mbits);
    }

    static void JUIsetColorScheme(int id){
        JUIsetColorSchemeNative(id);
        if(on_broadcast) operateClient.setTuneParams(TuneMsg.TuneType.COLOR_SCHEME, id);
    }

    public static void JUIonSingleTouchDown(float x, float y){
        JUIonSingleTouchDownNative(x, y);
        if(on_broadcast) operateClient.setGestureOp(GestureOp.OPType.TOUCH_DOWN, x, y);
    }
    public static void JUIonTouchMove(float x, float y){
        JUIonTouchMoveNative(x, y);
        if(on_broadcast) operateClient.setGestureOp(GestureOp.OPType.TOUCH_MOVE, x, y);
    }
    public static void JUIonScale(float sx, float sy){
        JUIonScaleNative(sx, sy);
        if(on_broadcast) operateClient.setGestureOp(GestureOp.OPType.SCALE, sx, sy);
    }
    public static void JUIonPan(float x, float y){
        JUIonPanNative(x, y);
        if(on_broadcast) operateClient.setGestureOp(GestureOp.OPType.PAN, x, y);
    }

    public static native void JUIonResetNative(int num, String[] check_keys, boolean[] check_value, float[] volume_pose, float[] camera_pose);
    public static native void JUIsetGraphRectNative(int id, int width, int height, int left, int top);

    public static native void JUIaddTuneParamsNative(int[] nums, float[] values);
    public static native void JUIsetTuneWidgetByIdNative(int wid);
    public static native void JUIremoveTuneWidgetByIdNative(int wid);
    public static native void JUIremoveAllTuneWidgetNative();
    public static native void JUIsetTuneWidgetVisibilityNative(int wid, boolean visible);
    public static native void JUIsetAllTuneParamByIdNative(int tid, float[] values);
    public static native void JUIsetTuneParamByIdNative(int tid, int pid, float value);
    public static native void JUIsetDualParamByIdNative(int pid, float minValue, float maxValue);

    public static native void JUIsetChecksNative(String key, boolean value);

    public static native float[] JUIgetVCStatesNative();
    public static native void JUIsetCuttingPlaneNative(int id, float value);
    public static native float[] JUIgetCuttingPlaneStatusNative();
    public static native void JUIsetMaskBitsNative(int num, int mbits);
    public static native void JUIsetColorSchemeNative(int id);

    //touch action
    public static native void JUIonSingleTouchDownNative(float x, float y);
    public static native void JUIonTouchMoveNative(float x, float y);
    public static native void JUIonScaleNative(float sx, float sy);
    public static native void JUIonPanNative(float x, float y);
}

package helmsley.vr.DUIs;

import helmsley.vr.proto.GestureOp;
import helmsley.vr.proto.operateClient;

public class JUIInterface {
    public static void JUIonReset(int num, String[] check_keys, boolean[] check_value, float[] volume_pose, float[] camera_pose){
        JUIonResetNative(num, check_keys, check_value, volume_pose, camera_pose);
    }
    static void JUIsetGraphRect(int id, int width, int height, int left, int top){
        JUIsetGraphRectNative(id, width, height, left, top);
    }

    static void JUIaddTuneParams(int[] nums, float[] values){
        JUIaddTuneParamsNative(nums, values);
    }
    static void JUIsetTuneWidgetById(int wid){
        JUIsetTuneWidgetByIdNative(wid);
    }
    static void JUIremoveTuneWidgetById(int wid){
        JUIremoveTuneWidgetByIdNative(wid);
    }
    static void JUIremoveAllTuneWidget(){
        JUIremoveAllTuneWidgetNative();
    }
    static void JUIsetTuneWidgetVisibility(int wid, boolean visible){
        JUIsetTuneWidgetVisibilityNative(wid, visible);
    }
    static void JUIsetAllTuneParamById(int tid, float[] values){
        JUIsetAllTuneParamByIdNative(tid, values);
    }
    static void JUIsetTuneParamById(int tid, int pid, float value){
        JUIsetTuneParamByIdNative(tid, pid, value);
    }
    public static void JUIsetDualParamById(int pid, float minValue, float maxValue){
        JUIsetDualParamByIdNative(pid, minValue,maxValue);
    }

    static void JUIsetChecks(String key, boolean value){
        JUIsetChecksNative(key, value);
    }

    public static float[] JUIgetVCStates(){
        return JUIgetVCStatesNative();
    }
    static void JUIsetCuttingPlane(int id, float value){
        JUIsetCuttingPlaneNative(id, value);
    }
    static float[] JUIgetCuttingPlaneStatus(){
        return JUIgetCuttingPlaneStatusNative();
    }
    static void JUIsetMaskBits(int num, int mbits){
        JUIsetMaskBitsNative(num, mbits);
    }
    static void JUIsetColorScheme(int id){
        JUIsetColorSchemeNative(id);
    }

    public static void JUIonSingleTouchDown(float x, float y){
        JUIonSingleTouchDownNative(x, y);
        operateClient.setGestureOp(GestureOp.OPType.TOUCH_DOWN, x, y);
    }
    public static void JUIonTouchMove(float x, float y){
        JUIonTouchMoveNative(x, y);
        operateClient.setGestureOp(GestureOp.OPType.TOUCH_MOVE, x, y);
    }
    public static void JUIonScale(float sx, float sy){
        JUIonScaleNative(sx, sy);
        operateClient.setGestureOp(GestureOp.OPType.SCALE, sx, sy);
    }
    public static void JUIonPan(float x, float y){
        JUIonPanNative(x, y);
        operateClient.setGestureOp(GestureOp.OPType.PAN, x, y);

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

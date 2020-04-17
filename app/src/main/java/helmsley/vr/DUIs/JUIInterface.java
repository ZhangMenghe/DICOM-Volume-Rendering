package helmsley.vr.DUIs;

public class JUIInterface {
    public static native void JUIResetValues(int tid, float[] values);
    public static native void JUIonReset(int num, String[] key, boolean[] value);
    public static native void JUIAddTuneParams(int[] nums, float[] values);
    public static native void JuisetGraphRect(int id, int width, int height, int left, int top);

    public static native void JUIsetTuneWidgetById(int wid);
    public static native void JUIremoveTuneWidgetById(int wid);
    public static native void JUIremoveAllTuneWidget();
    public static native void JUIsetTuneParamById(int wid, int pid, float value);
    public static native void JUIsetDualParamById(int pid, float minValue, float maxValue);

    public static native void JUIsetChecks(String key, boolean value);

    public static native void JUIsetCuttingPlane(int id, float value);
    public static native void JUIsetMaskBits(int num, int mbits);
    public static native void JuisetColorScheme(int id);

    //touch action
    public static native void JUIonSingleTouchDown(float x, float y);
    public static native void JUIonTouchMove(float x, float y);
    public static native void JUIonScale(float sx, float sy);
    public static native void JUIonPan(float x, float y);
}

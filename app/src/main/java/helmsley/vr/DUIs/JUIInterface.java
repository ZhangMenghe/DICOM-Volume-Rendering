package helmsley.vr.DUIs;

public class JUIInterface {
    public static native void JUIonReset(int num, String[] key, boolean[] value);
    public static native void JUIInitTuneParam(int id, int num, String[] key, float[] value);
    public static native void JUIInitCheckParam(int num, String[] key, boolean[] value);
    public static native void JuisetGraphRect(int id, int width, int height, int left, int top);

    public static native void JUIsetTuneParam(int id, String key, float value);
    public static native void JUIsetChecks(String key, boolean value);
    public static native void JUIsetCheckById(int id, boolean value);

    public static native void JUIsetCuttingPlane(int id, float value);
    public static native void JUIsetMaskBits(int num, int mbits);
    public static native void JuisetColorScheme(int id);

    //touch action
    public static native void JUIonSingleTouchDown(float x, float y);
    public static native void JUIonTouchMove(float x, float y);
    public static native void JUIonScale(float sx, float sy);
    public static native void JUIonPan(float x, float y);
}

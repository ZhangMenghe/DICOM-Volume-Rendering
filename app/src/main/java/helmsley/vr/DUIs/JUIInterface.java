package helmsley.vr.DUIs;

public class JUIInterface {
    public static native void JUIInitTuneParam(int id, int num, String[] key, float[] value);
    public static native void JUIInitCheckParam(int num, String[] key, boolean[] value);

    public static native void JUIsetTuneParam(int id, String key, float value);
    public static native void JUIsetChecks(String key, boolean value);

    public static native void JUIsetCuttingPlane(int id, float value);

    //touch action
    public static native void JUIonSingleTouchDown(float x, float y);
    public static native void JUIonTouchMove(float x, float y);
    public static native void JUIonScale(float sx, float sy);
    public static native void JUIonPan(float x, float y);
}

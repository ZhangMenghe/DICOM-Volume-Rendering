package helmsley.vr.DUIs;

public class JUIInterface {
    public static native void JUIInitTuneParam(int id, int num, String[] key, float[] value);
    public static native void JUIInitCheckParam(int num, String[] key, boolean[] value);

    public static native void JUIsetTuneParam(String key, float value);
    public static native void JUIsetChecks(String key, boolean value);
}

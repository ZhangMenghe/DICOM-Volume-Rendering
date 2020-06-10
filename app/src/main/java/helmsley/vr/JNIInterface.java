package helmsley.vr;

import android.content.res.AssetManager;

public class JNIInterface {
    public static native long JNIonCreate(AssetManager asset_manager);
    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonSurfaceChanged(int width, int height);
    public static native void JNIdrawFrame();

    public static native void JNIsendDataPrepare(int width, int height, int dims, float thicknes, boolean b_wmask);
    public static native void JNIsendData(int target, int idx, int chunk_size, int unit_size, byte[] data);
    public static native void JNIsendDataDone();
    public static native void JNIreleaseBuffer();
    public static native byte[] JNIgetVolumeData();
}
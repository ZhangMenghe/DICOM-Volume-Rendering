package helmsley.vr;

import android.content.res.AssetManager;

public class JNIInterface {
    public static native long JNIonCreate(AssetManager asset_manager);
    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonSurfaceChanged(int width, int height);
    public static native void JNIdrawFrame();

    public static native void JNIsendData(int target, int idx, int chunk_size, int unit_size, byte[] data);
    public static native void JNIsendDCMImg(int idx, int chunk_size, byte[] data);
    public static native void JNIsendDCMIMask(int idx, int chunk_size, byte[] data);
    public static native void JNIsetupDCMIConfig(int width, int height, int dims);
    public static native void JNIAssembleVolume();
    public static native void JNIAssembleMask();
    public static native byte[] JNIgetVolumeData(boolean b_getmask);
}
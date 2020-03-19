package helmsley.vr;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;

public class JNIInterface {
    public static native long JNIonCreate(AssetManager asset_manager);
    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonPause();
    public static native void JNIonResume(Context context, Activity activity);
    public static native void JNIonDestroy();
    public static native void JNIonSurfaceChanged(int rotation, int width, int height);
    public static native void JNIdrawFrame();

    public static native void JNIsendData(int target, int idx, int chunk_size, int unit_size, byte[] data);
    public static native void JNIsetupDCMIConfig(int width, int height, int dims, boolean b_wmask);
    public static native void JNIAssembleVolume();
    public static native byte[] JNIgetVolumeData();
}
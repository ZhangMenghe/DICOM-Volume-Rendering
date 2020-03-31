package helmsley.vr;

import android.content.res.AssetManager;
import android.graphics.Bitmap;

import helmsley.vr.Utils.dcmImage;

public class JNIInterface {
    public static native long JNIonCreate(AssetManager asset_manager);
    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonSurfaceChanged(int width, int height);
    public static native void JNIdrawFrame();

    public static native void JNIsendDCMImgs(dcmImage img_arr[], Object mask_arr[], int size);
    public static native void JNIsendDCMImg(int idx, int chunk_size, byte[] data);
    public static native void JNIsendDCMIMask(int idx, int chunk_size, byte[] data);
    public static native void JNIsetupDCMIConfig(int width, int height, int dims);
    public static native void JNIAssembleVolume();
    public static native void JNIAssembleMask();
    public static native byte[] JNIgetVolumeData(boolean b_getmask);
}
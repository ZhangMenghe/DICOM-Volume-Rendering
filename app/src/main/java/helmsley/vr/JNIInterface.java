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
}

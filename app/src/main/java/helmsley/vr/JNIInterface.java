package helmsley.vr;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.IOException;

import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.proto.operateClient;

public class JNIInterface {
    private static final String TAG = "JNIInterface";
    static AssetManager assetManager;

    public static native long JNIonCreate(AssetManager asset_manager);
    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonPause();
    public static native void JNIonResume(Context context, Activity activity);
    public static native void JNIonDestroy();
    public static native void JNIonSurfaceChanged(int rotation, int width, int height);
    public static native void JNIdrawFrame();

    static void JNIsendDataPrepare(int ph, int pw, int pd, float sh, float sw, float sd, boolean b_mask){
        JNIsendDataPrepareNative(ph, pw, pd, sh, sw, sd, b_mask);
    }
    public static void JNIsendDataPrepare(String target_vol, int ph, int pw, int pd, float sh, float sw, float sd, boolean b_mask){
        JNIsendDataPrepareNative(ph, pw, pd, sh, sw, sd, b_mask);
        operateClient.setDisplayVolume(target_vol, ph, pw, pd, sh, sw, sd, b_mask);
    }
    public static native void JNIsendData(int target, int idx, int chunk_size, int unit_size, byte[] data);
    public static native void JNIsendDataFloats(int target, int chunk_size, float[] data);
    public static native void JNIsendDataDone();
    public static native void JNIreleaseBuffer();
    public static native void JNIsendDataPrepareNative(int ph, int pw, int pd, float sh, float sw, float sd, boolean b_mask);

    public static native byte[] JNIgetVolumeData();
    public static Bitmap loadImage(String imageName) {
        try {
            return BitmapFactory.decodeStream(assetManager.open(imageName));
        } catch (IOException e) {
            Log.e(TAG, "Cannot open image " + imageName);
            return null;
        }
    }

    public static void loadTexture(int target, Bitmap bitmap) {
        GLUtils.texImage2D(target, 0, bitmap, 0);
    }
}
package helmsley.vr.Utils;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

/** Helper to ask camera permission. */
public class PermissionHelper {
    private static final String CAMERA_PERMISSION = Manifest.permission.CAMERA;
    private static final String AUDIO_RECORD_PERMISSION = Manifest.permission.RECORD_AUDIO;

    public static final int CAMERA_PERMISSION_CODE = 456;
    public static final int FILE_PERMISSION_CODE = 123;
    public static final int AUDIO_PERMISSION_CODE = 789;

    public static boolean[] checkAllPermissions(Activity activity){
        boolean[] grantes ={
          true, true, true
        };
        String[] FILE_PERMISSIONS = {
                android.Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
        };
        for(String rpermission:FILE_PERMISSIONS){
            if(ActivityCompat.checkSelfPermission(activity, rpermission) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(activity, FILE_PERMISSIONS, FILE_PERMISSION_CODE);
                grantes[0] = false;
            }
        }
        if(!hasCameraPermission(activity)){
            grantes[1] = false;
            requestCameraPermission(activity);
        }
        if(!hasAudioRecordPermission(activity)){
            grantes[2] = false;
            requestAudioRecordPermission(activity);
        }
        return grantes;
    }

    /** Check to see we have the necessary permissions for this app. */
    public static boolean hasCameraPermission(Activity activity) {
        return ContextCompat.checkSelfPermission(activity, CAMERA_PERMISSION)
                == PackageManager.PERMISSION_GRANTED;
    }

    /** Check to see we have the necessary permissions for this app, and ask for them if we don't. */
    public static void requestCameraPermission(Activity activity) {
        ActivityCompat.requestPermissions(
                activity, new String[] {CAMERA_PERMISSION}, CAMERA_PERMISSION_CODE);
    }

    public static boolean hasAudioRecordPermission(Activity activity) {
        return ContextCompat.checkSelfPermission(activity, AUDIO_RECORD_PERMISSION)
                == PackageManager.PERMISSION_GRANTED;
    }

    public static void requestAudioRecordPermission(Activity activity) {
        ActivityCompat.requestPermissions(
                activity, new String[] {AUDIO_RECORD_PERMISSION}, AUDIO_PERMISSION_CODE);
    }

    /** Check to see if we need to show the rationale for this permission. */
    public static boolean shouldShowRequestPermissionRationale(Activity activity) {
        return ActivityCompat.shouldShowRequestPermissionRationale(activity, CAMERA_PERMISSION);
    }

    /** Launch Application Setting to grant permission. */
    public static void launchPermissionSettings(Activity activity) {
        Intent intent = new Intent();
        intent.setAction(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        intent.setData(Uri.fromParts("package", activity.getPackageName(), null));
        activity.startActivity(intent);
    }
}
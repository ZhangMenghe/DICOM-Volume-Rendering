package helmsley.vr;

import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

import helmsley.vr.Utils.CameraPermissionHelper;

public class MainActivity extends GLActivity
    implements DisplayManager.DisplayListener{

    final static String TAG = MainActivity.class.getSimpleName();
    static {
        System.loadLibrary("vrAndroid");
    }
    //ui
    protected UIsManager ui_manager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ui_manager = new UIsManager(this);
    }

    @Override
    protected void onResume() {

        if(Boolean.getBoolean(getString(R.string.ar_enable))){
            if (!CameraPermissionHelper.hasCameraPermission(this)) {
                CameraPermissionHelper.requestCameraPermission(this);
                return;
            }
//            JNIInterface.onResume(nativeAddr, getApplicationContext(), this);
        }

        super.onResume();
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }
    @Override
    protected void onPause(){
        super.onPause();
//        JNIInterface.JNIonPause();
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }
    @Override
    public void onDestroy(){
        super.onDestroy();
        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
//            JNIInterface.JNIonDestroy(nativeAddr);
            nativeAddr = 0;
        }
    }
    @Override
    protected void updateOnFrame(){
        ui_manager.updateOnFrame();
        super.updateOnFrame();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
//        super.onWindowFocusChanged(hasFocus);
//        if (hasFocus) {
//            // Standard Android full-screen functionality.
//            getWindow()
//                    .getDecorView()
//                    .setSystemUiVisibility(
//                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
//                                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
//                                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
//                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
//                                    | View.SYSTEM_UI_FLAG_FULLSCREEN
//                                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
//            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            Toast.makeText(this, "Camera permission is needed to run this application", Toast.LENGTH_LONG)
                    .show();
            if (!CameraPermissionHelper.shouldShowRequestPermissionRationale(this)) {
                // Permission denied with checking "Do not ask again".
                CameraPermissionHelper.launchPermissionSettings(this);
            }
            finish();
        }
    }
    // DisplayListener methods
    @Override
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {
        viewportChanged = true;
    }
}

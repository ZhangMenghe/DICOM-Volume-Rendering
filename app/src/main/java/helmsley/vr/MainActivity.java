package helmsley.vr;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import helmsley.vr.Utils.PermissionHelper;

public class MainActivity extends GLActivity
        implements DisplayManager.DisplayListener{

    final static String TAG = MainActivity.class.getSimpleName();
    static {
        System.loadLibrary("vrAndroid");
    }
    //ui
    protected UIsManager ui_manager;
    private dicomManager dcm_manager;
    public static boolean permission_granted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("imebra_lib");
        ui_manager = new UIsManager(this);
        dcm_manager = new dicomManager(this);
    }

    @Override
    protected void onResume() {
        super.onResume();

        //camera for ar
        if(PermissionHelper.checkAllPermissions(this)[1]){
            if(Boolean.parseBoolean(getString(R.string.ar_enable))){
                try{
                    JNIInterface.JNIonResume(getApplicationContext(), this);
                }catch (Exception e){
                    Log.e(TAG, "====onResume error I can't solve" );
                }
            }
        }

        surfaceView.onResume();
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }
    @Override
    protected void onPause(){
        super.onPause();
        JNIInterface.JNIonPause();
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }
    @Override
    public void onDestroy(){
        super.onDestroy();
        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            JNIInterface.JNIonDestroy();
            nativeAddr = 0;
        }
    }

    @Override
    protected void updateOnFrame(){
        ui_manager.updateOnFrame();
        dcm_manager.updateOnFrame();
        super.updateOnFrame();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        //override, do nothing
    }
    /*
    Here we get the response from the file selector. We use the returned URI to open an
    InputStream which we push to the DICOM codec through a PIPE.

    It would be simpler to just use a file name with the DICOM codec, but this is difficult
    to obtain from the file selector dialog and would not allow to load also files from external
    sources (e.g. the Google Drive).
 */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(requestCode == PermissionHelper.FILE_PERMISSION_CODE && resultCode == RESULT_OK)
            dcm_manager.Run(data.getData());
    }

    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults ){
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if(requestCode  == PermissionHelper.FILE_PERMISSION_CODE){
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                permission_granted = true;
            } else {
                Toast.makeText(
                        this,
                        "Permission denied to read your External storage, try to enable it via system setting",
                        Toast.LENGTH_SHORT
                ).show();
            }
        }else if(requestCode == PermissionHelper.CAMERA_PERMISSION_CODE){
            if (PermissionHelper.hasCameraPermission(this)) {

                if(Boolean.parseBoolean(getString(R.string.ar_enable))){
                    try{
                        JNIInterface.JNIonResume(getApplicationContext(), this);
                    }catch (Exception e){
                        Log.e(TAG, "====onResume error I can't solve" );
                    }
                }

            }else{
                Toast.makeText(this, "Camera permission is needed to run this application", Toast.LENGTH_LONG)
                        .show();
            }
        }else{
            if (!PermissionHelper.hasAudioRecordPermission(this)) {
                Toast.makeText(this, "Audio recorder permission is needed to run this application", Toast.LENGTH_LONG)
                        .show();
//                if (!PermissionHelper.shouldShowRequestPermissionRationale(this)) {
//                    // Permission denied with checking "Do not ask again".
//                    PermissionHelper.launchPermissionSettings(this);
//                }
//                finish();
            }
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

package helmsley.vr;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.util.Date;

import helmsley.vr.Utils.CameraPermissionHelper;

public class MainActivity extends GLActivity
        implements DisplayManager.DisplayListener{

    final static String TAG = MainActivity.class.getSimpleName();
    public static final int FILE_PERMISSION_CODE = 123;
    static {
        System.loadLibrary("vrAndroid");
    }
    //ui
    protected UIsManager ui_manager;
    private dicomManager dcm_manager;
    public static boolean permission_granted = false;

    //debug recording
    private boolean mIsRecording = false;
    private File mOutputFile;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("imebra_lib");
        ui_manager = new UIsManager(this);
        dcm_manager = new dicomManager(this);

        Button btn = findViewById(R.id.record_button);

        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mIsRecording) {
                    surfaceView.stopRecording();
//                    share();
                    mIsRecording = false;

                    mOutputFile = createVideoOutputFile();

                    try {
                        int screenWidth = surfaceView.getWidth();
                        int screenHeight = surfaceView.getHeight();
                        surfaceView.initRecorder(mOutputFile, (int) screenWidth, (int) screenHeight, null,
                                null);
                    } catch (IOException ioex) {
                        Log.e(TAG, "Couldn't re-init recording", ioex);
                    }
                    ((Button)v).setText("Record");

                } else {

                    surfaceView.startRecording();
//                    Log.v(TAG, "Recording Started");
//                    ((Button)v).setText("Stop");
                    mIsRecording = true;

                }
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(Boolean.parseBoolean(getString(R.string.ar_enable))){
            if (!CameraPermissionHelper.hasCameraPermission(this)) {
                CameraPermissionHelper.requestCameraPermission(this);
                return;
            }
            try{
                JNIInterface.JNIonResume(getApplicationContext(), this);
            }catch (Exception e){
                Log.e(TAG, "====onResume error I can't solve" );
            }
        }

        surfaceView.onResume();

        try {
            mOutputFile = createVideoOutputFile();
            android.graphics.Point size = new android.graphics.Point();
            getWindowManager().getDefaultDisplay().getRealSize(size);
            surfaceView.initRecorder(mOutputFile, size.x, size.y, null, null);
        } catch (IOException ioex) {
            Log.e(TAG, "Couldn't re-init recording", ioex);
        }

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
        if(requestCode == FILE_PERMISSION_CODE && resultCode == RESULT_OK)
            dcm_manager.Run(data.getData());
    }
    @Override
    protected void checkPermissions(){
        String[] PERMISSIONS = {
                android.Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
        };
        for(String rpermission:PERMISSIONS){
            if(ActivityCompat.checkSelfPermission(this, rpermission) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS, FILE_PERMISSION_CODE);
                return;
            }
        }
        permission_granted = true;
//        fileUtils.writeFileToExternalStorage();
    }
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults ){
        if(requestCode  == FILE_PERMISSION_CODE){
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                permission_granted = true;
            } else {
                Toast.makeText(
                        this,
                        "Permission denied to read your External storage, try to enable it via system setting",
                        Toast.LENGTH_SHORT
                ).show();
            }
        }else if(requestCode == CameraPermissionHelper.CAMERA_PERMISSION_CODE){
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
    }
    private void share() {
        Uri contentUri = FileProvider.getUriForFile(getApplicationContext(),
        "com.example.android.opengl.fileprovider", mOutputFile);

        Intent shareIntent = new Intent();
        shareIntent.setAction(Intent.ACTION_SEND);
        shareIntent.setType("video/mp4");
        shareIntent.putExtra(Intent.EXTRA_STREAM, contentUri);
        shareIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(Intent.createChooser(shareIntent, "Share with"));
    }
    private File createVideoOutputFile() {

        File tempFile = null;
//        try {
//            File dirCheck = new File(
//                    getFilesDir().getCanonicalPath() + "/" + "captures");
//
//            if (!dirCheck.exists()) {
//                dirCheck.mkdirs();
//            }

            String filename = new Date().getTime() + "";
            tempFile = new File(Environment.getExternalStoragePublicDirectory(
                    Environment.DIRECTORY_MOVIES), filename+".mp4");

//            tempFile = new File(
//                    getFilesDir().getCanonicalPath() + "/" + "captures" + "/"
//                            + filename + ".mp4");
//        } catch (IOException ioex) {
//            Log.e(TAG, "Couldn't create output file", ioex);
//        }

        return tempFile;

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

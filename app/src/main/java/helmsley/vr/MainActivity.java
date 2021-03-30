package helmsley.vr;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Toast;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.videoio.VideoWriter;

import java.io.File;
import java.util.Arrays;

import helmsley.vr.Utils.AVIRecorder;
import helmsley.vr.Utils.CameraPermissionHelper;
import helmsley.vr.Utils.fileUtils;

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
//        debugVideoWriter();
        debugJNIVideoWriter();

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
    void debugJNIVideoWriter(){
        File file = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_MOVIES), "videoWriter.avi");
        AVIRecorder.onStartRecording(file.getAbsolutePath());
        byte[] data = new byte[640 * 480 * 3];
        Arrays.fill(data, (byte)255);
        for (int i = 0; i < 1000; i++) {
            AVIRecorder.WriteToVideo(data);
        }
        AVIRecorder.onStopRecording();
    }
    void debugVideoWriter(){
        File file = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_MOVIES), "videoWriter.avi");
        String filePath = file.getAbsolutePath();
        double FPS = 30.0;
        int w = 460, h=640;

        VideoWriter videoWriter = new VideoWriter(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w,h));
        videoWriter.open(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w,h));

        if(!videoWriter.isOpened()){
            Log.e(TAG, "======debugVideoWriter: fail to open" );
        }else{
            for (int i = 0; i < 1000; i++) {
//            byte[] image = images.get(i);
                Mat rgbMat = new Mat(h,w, CvType.CV_8UC3);
                rgbMat.setTo(new Scalar(255,0,0));
                videoWriter.write(rgbMat);
                rgbMat.release();
            }
        }
        videoWriter.release();
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

package helmsley.vr;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import helmsley.vr.Record.RecordableGLRenderer;
import helmsley.vr.Record.RecordableSurfaceView;
import helmsley.vr.Utils.GestureDetectorCalVR;
import helmsley.vr.Utils.fileUtils;

public class GLActivity extends AppCompatActivity {
    final static String TAG = "BASIC_Activity";
    // Resource
    //SET ME!! folder name in asset folder that would be copied to runtime folder
    protected String ass_copy_src;
    //DST folder that runtime can use, constains what u copied from asset
    protected String ass_copy_dst = null;

    final static boolean skipLoadingResource = true;


    // Opaque native pointer to the native application instance.

    //Surface view
    protected RecordableSurfaceView surfaceView;
    protected long nativeAddr;

    //For touch event
    protected GestureDetectorCalVR gestureDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);
        checkPermissions();
        setupSurfaceView();
        JNIInterface.assetManager = getAssets();
        nativeAddr = JNIInterface.JNIonCreate(JNIInterface.assetManager);
        setupTouchDetector();
    }
    protected void checkPermissions(){}
    @Override
    protected void onResume() {
        super.onResume();
        surfaceView.onResume();
    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
    }
    @Override
    public void onDestroy(){
        super.onDestroy();
    }
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            // Standard Android full-screen functionality.
            getWindow()
                    .getDecorView()
                    .setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }
    private void setupTouchDetector() {
        gestureDetector = new GestureDetectorCalVR(this);

        surfaceView.setOnTouchListener(
                new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent e) {
                        return gestureDetector.onTouchEvent(e);
                    }
                }
        );
    }

    private void setupSurfaceView(){
        surfaceView = (RecordableSurfaceView) findViewById(R.id.surfaceview);
        surfaceView.setupGLActivityRef(this);
//        surfaceView.onInitialize();
//        // Set up renderer.
//        surfaceView.setPreserveEGLContextOnPause(true);
//        surfaceView.setEGLContextClientVersion(3);
//        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
//        surfaceView.setRenderer(new GLActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    protected void copyFromAssets(){
        ass_copy_src = getString(R.string.cf_cache_folder_name);
        ass_copy_dst = getFilesDir().getAbsolutePath() + "/" + ass_copy_src;

        File destDir = new File(ass_copy_dst);

        //Skip copying if files exist
        if(skipLoadingResource && destDir.exists()) return;
        fileUtils.deleteDirectory(destDir);
        try{
            fileUtils.copyFromAsset(getAssets(), ass_copy_src, ass_copy_dst);
        }catch (Exception e){
            Log.e(TAG, "copyFromAssets: Failed to copy from asset folder");
        }
    }

    public boolean setupResource(){
        copyFromAssets();
        return true;
    }
    public boolean updateOnFrame(boolean viewportChanged, int viewportWidth, int viewportHeight){
        if(nativeAddr == 0)return false;
        if (viewportChanged) {
            int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
            JNIInterface.JNIonSurfaceChanged(displayRotation, viewportWidth, viewportHeight);
        }
        return true;
    }
}

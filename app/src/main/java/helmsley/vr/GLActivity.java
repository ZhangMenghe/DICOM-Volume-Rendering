package helmsley.vr;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import helmsley.vr.Utils.GestureDetectorCalVR;
import helmsley.vr.Utils.fileUtils;

public class GLActivity extends AppCompatActivity {
    final static String TAG = "BASIC_Activity";
    // Resource
    //SET ME!! folder name in asset folder that would be copied to runtime folder
    protected String ass_copy_src;
    //DST folder that runtime can use, constains what u copied from asset
    protected String ass_copy_dst = null;

    final static boolean skipLoadingResource = false;

    //Surface view
    protected GLSurfaceView surfaceView;
    protected long nativeAddr;

    //For touch event
    protected GestureDetectorCalVR gestureDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);
        setupSurfaceView();
        nativeAddr = JNIInterface.JNIonCreate(getAssets());
        setupTouchDetector();
    }

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
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(3);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new GLActivity.Renderer());
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

    protected boolean setupResource(){
        copyFromAssets();
        return true;
    }
    protected void updateOnFrame(){}
    protected class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            if(setupResource()){
                JNIInterface.JNIonGlSurfaceCreated();
            }
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            JNIInterface.JNIonSurfaceChanged(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            JNIInterface.JNIdrawFrame();
            updateOnFrame();
        }
    }
}

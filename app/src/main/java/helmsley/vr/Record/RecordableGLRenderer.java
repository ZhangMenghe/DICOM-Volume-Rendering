package helmsley.vr.Record;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.Log;

import java.lang.ref.WeakReference;

import helmsley.vr.GLActivity;
import helmsley.vr.JNIInterface;

/**
 * Provides drawing instructions for a GLSurfaceView object. This class
 * must override the OpenGL ES drawing lifecycle methods:
 * <ul>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onSurfaceCreated}</li>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onDrawFrame}</li>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onSurfaceChanged}</li>
 * </ul>
 */
public class RecordableGLRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "MyGLRenderer";
    private WeakReference<GLActivity> actRef = null;
    protected boolean viewportChanged = false;
    protected int viewportWidth;
    protected int viewportHeight;
    public void setupGLActivityRef(GLActivity activity){
        actRef = new WeakReference<>(activity);
    }
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        if(actRef.get().setupResource())
            JNIInterface.JNIonGlSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        viewportChanged = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
//        // Synchronized to avoid racing onDestroy.
        if(!actRef.get().updateOnFrame(viewportChanged, viewportWidth, viewportHeight)) return;
//        synchronized (this) {
//            viewportChanged = false;
//            JNIInterface.JNIdrawFrame();
//        }
        viewportChanged = false;
        JNIInterface.JNIdrawFrame();
    }
    public void dirtyViewport(){viewportChanged = true;}
}

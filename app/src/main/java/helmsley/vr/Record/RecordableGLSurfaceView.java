package helmsley.vr.Record;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;

/**
 * A view container where OpenGL ES graphics can be drawn on screen.
 * This view can also be used to capture touch events, such as a user
 * interacting with drawn objects.
 */
public class RecordableGLSurfaceView extends RecordableSurfaceView implements RecordableSurfaceView.RendererCallbacks {
    private RecordableGLRenderer mRenderer;
    public RecordableGLSurfaceView(Context context) {
        super(context);

        // Set the Renderer for drawing on the GLSurfaceView
        mRenderer = new RecordableGLRenderer();
        setRendererCallbacks(this);
    }
    public RecordableGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // Set the Renderer for drawing on the GLSurfaceView
        mRenderer = new RecordableGLRenderer();
        setRendererCallbacks(this);
    }

    public void onInitialize(){
        // Set the Renderer for drawing on the GLSurfaceView
        mRenderer = new RecordableGLRenderer();
        setRendererCallbacks(this);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        return false;
    }

    @Override
    public void onSurfaceCreated() {
        mRenderer.onSurfaceCreated(null, null);
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        mRenderer.onSurfaceChanged(null, width, height);
    }

    @Override
    public void onSurfaceDestroyed() {

    }

    @Override
    public void onContextCreated() {

    }

    @Override
    public void onPreDrawFrame() {

    }

    @Override
    public void onDrawFrame() {
        mRenderer.onDrawFrame(null);
    }
}

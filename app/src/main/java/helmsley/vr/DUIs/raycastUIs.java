package helmsley.vr.DUIs;

import android.app.Activity;

import java.lang.ref.WeakReference;

public class raycastUIs {
    private final WeakReference<Activity> activityReference;
    final static String TAG = "raycastUIs";
    public raycastUIs(final Activity activity_){
        activityReference = new WeakReference<>(activity_);
    }

}

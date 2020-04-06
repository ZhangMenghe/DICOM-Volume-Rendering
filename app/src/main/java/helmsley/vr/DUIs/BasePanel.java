package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.View;
import android.view.ViewGroup;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;

public abstract class BasePanel {
    final WeakReference<Activity> actRef;
    final WeakReference<ViewGroup> parentRef;
    int default_check_array_id;
    int default_check_values_id;
    String[] check_names_;

    ArrayList<View> sub_panels_ = new ArrayList<>();
    boolean panel_visible;
    BasePanel(final Activity activity, ViewGroup parent_view){
        actRef = new WeakReference<>(activity);
        parentRef = new WeakReference<>(parent_view);
        default_check_array_id = 0;
        default_check_values_id = 0;
    }
    public void showHidePanel(boolean isPanelOn){
        if(panel_visible && !isPanelOn)
            for(View v:sub_panels_)parentRef.get().removeView(v);
        else if(!panel_visible && isPanelOn)
            for(View v:sub_panels_)parentRef.get().addView(v);
        panel_visible = isPanelOn;
    }
    public void showHidePanel(boolean isPanelOn, boolean isRaycasting){
        showHidePanel(isPanelOn);
    }
    public void onTexRayChange(boolean israycasting){}
    public void Reset(){}
    public ArrayList<Boolean> setCheckParams(Resources res, ArrayList<Boolean> values){
        if(default_check_array_id!=0){
            check_names_ = res.getStringArray(default_check_array_id);
            TypedArray ta_values = res.obtainTypedArray(default_check_values_id);
            if(check_names_.length == ta_values.length())
                for(int i=0; i<check_names_.length;i++) values.add(ta_values.getBoolean(i, false));
            else
                Arrays.fill(check_names_, null);
            ta_values.recycle();
        }
        return values;
    }
}

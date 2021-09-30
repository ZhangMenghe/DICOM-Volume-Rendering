package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;

public abstract class BasePanel {
    final WeakReference<Activity> actRef;
    final WeakReference<ViewGroup> parentRef;
    private int default_check_array_id;
    private int default_check_values_id;
    CheckBox primary_checkbox = null;
    String[] check_names_;
    Boolean[] default_check_values = null;

    ArrayList<View> sub_panels_ = new ArrayList<>();
    boolean panel_visible;
    boolean default_primary_check;
    protected String main_check_name;

    BasePanel(final Activity activity, ViewGroup parent_view){
        actRef = new WeakReference<>(activity);
        parentRef = new WeakReference<>(parent_view);
        default_check_array_id = 0;
        default_check_values_id = 0;
        panel_visible = false;
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
    public LinkedHashMap getCurrentStates(){return null;}
    public void onRenderingMethodChange(int rm_id){}
    public void Reset(){}
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){}
    public void setCheckParams(ArrayList<String> names, ArrayList<Boolean> values){
        if(default_check_array_id==0) return;

        Collections.addAll(names, check_names_);
        Collections.addAll(values, default_check_values);
    }
    void setup_checks(int check_array_id, int check_value_id){
        default_check_array_id = check_array_id;
        default_check_values_id = check_value_id;
        check_names_ = actRef.get().getResources().getStringArray(default_check_array_id);
        setup_default_check_values();
    }
    void setup_checks(View parent_view, int check_array_id, int check_value_id, int main_checkbox_resid, int main_checkbox_index) {
        setup_checks(check_array_id, check_value_id);
        setup_jni_show_hide_checkbox(parent_view, main_checkbox_resid, main_checkbox_index);
    }
    void setup_default_check_values(){
        default_check_values = new Boolean[check_names_.length];
        TypedArray ta_values = actRef.get().getResources().obtainTypedArray(default_check_values_id);
        for(int i=0; i<check_names_.length;i++)
            default_check_values[i] = ta_values.getBoolean(i, false);
        ta_values.recycle();
    }
    private void setup_jni_show_hide_checkbox(View parent_view, int cb_id, int check_idx){
        Resources res = actRef.get().getResources();
        main_check_name = res.getStringArray(default_check_array_id)[check_idx];
        default_primary_check = default_check_values[check_idx];

        primary_checkbox = (CheckBox)parent_view.findViewById(cb_id);
        primary_checkbox.setChecked(default_primary_check);
        primary_checkbox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(main_check_name, isChecked);
            }
        });
    }
}

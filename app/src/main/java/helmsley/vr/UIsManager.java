package helmsley.vr;

import android.app.ActionBar;
import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Log;
import android.util.Pair;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import java.util.LinkedHashMap;

import helmsley.vr.DUIs.SeekbarAdapter;
import helmsley.vr.DUIs.checkboxAdapter;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";

    // UIs
    private Spinner spinner_tune, spinner_check, spinner_func;
    private Button button_checkg, button_tuneg;

    // value maps



    final private int tex_id=0, raycast_id=1;
    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        InitUIs();
    }


    private void InitUIs(){
        //check spinner
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);
        setupTuneSpinner();
        setupCheckBoxSpinner();
    }


    private void setupTuneSpinner(){
        spinner_tune =  (Spinner)actRef.get().findViewById(R.id.tuneSpinner);
        SeekbarAdapter seekbarAdapter = new SeekbarAdapter(actRef.get());
        spinner_tune.setAdapter(seekbarAdapter.createListAdapter(raycast_id));
    }
    private void setupCheckBoxSpinner(){
        spinner_check =  (Spinner)actRef.get().findViewById(R.id.checkSpinner);
        checkboxAdapter cbAdapter = new checkboxAdapter(actRef.get());
        spinner_check.setAdapter(cbAdapter.createListAdapter(tex_id));
    }
}

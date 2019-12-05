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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import helmsley.vr.DUIs.SeekbarAdapter;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";

    // UIs
    private Spinner spinner_tune, spinner_check, spinner_func;
    private Button button_checkg, button_tuneg;

    // value maps
    private LinkedHashMap<String, Boolean> check_map=new LinkedHashMap<>();


    final private int tex_id=0, raycast_id=1;
    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        SetupDefaultInitialSetting();
        InitUIs();
    }

    private void SetupDefaultInitialSetting(){
        Resources res = actRef.get().getResources();
        //setup check map values
        String check_items[] = res.getStringArray(R.array.checkParams);
        TypedArray check_values = res.obtainTypedArray(R.array.checkValues);
        for(int i=0; i<check_items.length; i++)
            check_map.put(check_items[i], check_values.getBoolean(i, false));
    }
    private void InitUIs(){
        //check spinner
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);
        setupTuneSpinner();
    }


    private void setupTuneSpinner(){
        spinner_tune =  (Spinner)actRef.get().findViewById(R.id.tuneSpinner);
        SeekbarAdapter seekbarAdapter = new SeekbarAdapter(actRef.get());
        spinner_tune.setAdapter(seekbarAdapter.createListAdapter(raycast_id));
    }
}

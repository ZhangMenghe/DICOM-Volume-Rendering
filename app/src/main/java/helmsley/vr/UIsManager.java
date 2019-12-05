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
    private ArrayList<LinkedHashMap<String, Float>> tune_maps= new ArrayList<>(2);

    private ArrayList<float[]>tune_maxs = new ArrayList<>();
    private ArrayList<int[]>tune_seek_max= new ArrayList<>();

    final private int tex_id=0, raycast_id=1;
    private final static String[] TuneTitles = {"Opacity", "Tunes"};
    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        SetupDefaultInitialSetting();
        InitUIs();
    }

    private void setupTuneMapValue(Resources res, int paramID) {
        TypedArray params = res.obtainTypedArray(paramID);
        int item_numbers = params.length();


        LinkedHashMap<String, Float> value_map = new LinkedHashMap<>();
        float[] max_values = new float[item_numbers];
        int[] max_seeks = new int[item_numbers];

        for (int i = 0; i < params.length(); i++) {
            int resId = params.getResourceId(i, -1);
            if (resId == -1) continue;
            String param[] = res.getStringArray(resId);
            value_map.put(param[0], Float.valueOf(param[1]));
            max_values[i] = Float.valueOf(param[2]);
            max_seeks[i] = Integer.valueOf(param[3]);
        }
        tune_maxs.add(max_values); tune_seek_max.add(max_seeks);tune_maps.add(value_map);
    }

    private void SetupDefaultInitialSetting(){
        Resources res = actRef.get().getResources();
        //setup check map values
        String check_items[] = res.getStringArray(R.array.checkParams);
        TypedArray check_values = res.obtainTypedArray(R.array.checkValues);
        for(int i=0; i<check_items.length; i++)
            check_map.put(check_items[i], check_values.getBoolean(i, false));

        //setup tune values
        setupTuneMapValue(res, R.array.texParams);
        setupTuneMapValue(res, R.array.raycastParams);
    }
    private void InitUIs(){
        //check spinner
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);
        setupTuneSpinner(TuneTitles[tex_id], tune_maps.get(tex_id), tune_maxs.get(tex_id), tune_seek_max.get(tex_id));
        UpdateCheckBoxesValue();
        UpdateTuneSeekBarsValue();
    }
    private void UpdateTuneSeekBarsValue(){

    }
    private void UpdateCheckBoxesValue(){

    }

    private void setupTuneSpinner(String title, LinkedHashMap vmap, float[] tune_max, int[] seek_max){
        spinner_tune =  (Spinner)actRef.get().findViewById(R.id.tuneSpinner);
        SeekbarAdapter seekbarAdapter = new SeekbarAdapter();
        spinner_tune.setAdapter(seekbarAdapter.getListAdapter(
                actRef.get(),
                new ArrayList<>(vmap.keySet()),
                new ArrayList<>(vmap.values()),
                tune_max,
                seek_max,
                title));
    }
}

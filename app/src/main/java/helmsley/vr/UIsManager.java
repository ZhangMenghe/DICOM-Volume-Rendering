package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Pair;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";

    // UIs
    private Spinner spinner_func;
    private Button button_checkg, button_tuneg;

    // value maps
    private Map<String, Boolean> check_map=new HashMap<>();
    private ArrayList<Map<String, Float>> tune_maps= new ArrayList<>(2);

    private ArrayList<float[]>tune_maxs = new ArrayList<>(2);
    private ArrayList<int[]>tune_seek_max= new ArrayList<>(2);

    final private int tex_id=0, raycast_id=1;

    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
//        SetupDefaultInitialSetting();
        InitUIs();
    }

    private void setupTuneMapValue(Resources res, int paramID, int tune_idx ) {
        TypedArray params = res.obtainTypedArray(paramID);
        int item_numbers = params.length();


        HashMap<String, Float> value_map = new HashMap<>();
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
        tune_maxs.set(tune_idx, max_values);
        tune_seek_max.set(tune_idx, max_seeks);
        tune_maps.set(tune_idx, value_map);
    }

    private void SetupDefaultInitialSetting(){
        Resources res = actRef.get().getResources();
        //setup check map values
        String check_items[] = res.getStringArray(R.array.checkParams);
        TypedArray check_values = res.obtainTypedArray(R.array.checkValues);
        for(int i=0; i<check_items.length; i++)
            check_map.put(check_items[i], check_values.getBoolean(i, false));

        //setup tune values
        setupTuneMapValue(res, R.array.texParams, tex_id);
        setupTuneMapValue(res, R.array.raycastParams, raycast_id);
    }
    private void InitUIs(){
        //check spinner
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);

        UpdateCheckBoxesValue();
        UpdateTuneSeekBarsValue();
    }
    private void UpdateTuneSeekBarsValue(){

    }
    private void UpdateCheckBoxesValue(){

    }
}

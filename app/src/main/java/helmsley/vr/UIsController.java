package helmsley.vr;

import android.app.Activity;

import android.util.Pair;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.AdapterView;

import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class UIsController {
    public static Activity activity;
    final static String TAG = "UIsController";

    // UIs
    public static TextView FPSlabel, toggleValueTex;

    private Spinner spinner_toggle_arr1, spinner_toggle_arr2, spinner_toggle_sub, spinner_switch;
    private SeekBar seekbar_top, seekbar_bottom;
    private Switch switch_widget;

    private View bottom_panel;
    private int toggle_id = 0, toggle_id_sub = 0, switch_id = 0;
    final private int SUB_OFFSET = 100;
    private Map<String, Float> toggle_values=new HashMap<>(), toggle_values_sub=new HashMap<>();
    private Map<String, Boolean> bool_values=new HashMap<>();

//    private List<float[]> tvalues = new ArrayList<float[]>();
    public static TranslateAnimation panelHiddenAction, panelShownAction;

    //array code -> key-value array
    //Important! Order matters!!Align with the resource file
    final private static Map<Integer, Pair<String,?>[] > ui_map= new HashMap<Integer, Pair<String,?>[]>() {{
        put(R.array.t1Arr, new Pair[] {new Pair("Opacity", -1.0f)});
        put(R.array.t2Arr, new Pair[] {new Pair("Opacity", -1.0f), new Pair("samplestep", 500.0f), new Pair("threshold", 0.4f), new Pair("brightness", 300.0f)});
        put(R.array.opacityArr, new Pair[] {new Pair("overall", 1.0f), new Pair("lowbound", 1.0f), new Pair("cutoff", .0f)});
        put(R.array.switchArr, new Pair[] {new Pair("colortrans", false),new Pair("raycast", false), new Pair("cutting", false), new Pair("simplecube", false)});
        put(R.array.bottomArr, new Pair[] {new Pair("cutting", .0f)});
    }};
    final private static Map<String, Pair<Float, Integer>> toggle_max_map = new HashMap<String, Pair<Float, Integer>>(){{
        put("samplestep", new Pair(800.0f, 800)); put("threshold", new Pair(2.0f, 800)); put("brightness", new Pair(500.0f, 800)); put("cutting", new Pair(1.0f, 800));
    }};

    final private static Pair<Float, Integer> toggle_max_sub_pair = new Pair(1.0f, 50);

    private List<String> entry_has_sub_arrs;
    private int current_toggle_id;

    static {
        panelHiddenAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF,
                0.0f, Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                -1.0f);
        panelHiddenAction.setDuration(500);

        panelShownAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                -1.0f, Animation.RELATIVE_TO_SELF, 0.0f);
        panelShownAction.setDuration(500);
    }
    static long startTime = System.nanoTime();
    static int frames = 0;

    public UIsController(final Activity activity_) {
        activity = activity_;
        SetupDefaultInitialSetting();
        Initialize();
    }

    private void SetupDefaultInitialSetting(){
        entry_has_sub_arrs = Arrays.asList(activity.getResources().getStringArray(R.array.entry_has_sub));

        for(Map.Entry entry:ui_map.entrySet()){
            Pair[] pair_arr =(Pair[]) entry.getValue(); //activity.getResources().getStringArray(R.array.t2Arr);
            for(Pair par:pair_arr){
                if(entry_has_sub_arrs.contains(par.first)) continue;
                switch ((int) entry.getKey()){
                    case R.array.t2Arr:
                    case R.array.bottomArr:
                        toggle_values.put((String)par.first, (Float)par.second);
                        JUIsetParam((String)par.first, (float)par.second);
                        break;
                    case R.array.opacityArr:
                        toggle_values_sub.put((String)par.first, (Float)par.second);
                        JUIsetParam((String)par.first, (float)par.second);
                        break;
                    case R.array.switchArr:
                        bool_values.put((String)par.first, (Boolean) par.second);
                        JUIsetSwitches((String)par.first, (boolean)par.second);
                        break;
                    default:break;
                }
            }
        }
    }
    private void updateSpinnerEnteries(){
        Pair<String, ?>[] arr = ui_map.get(R.array.switchArr);
        boolean arr1_sel = bool_values.get((String) arr[1].first);
        if(arr1_sel){
            spinner_toggle_arr1.setVisibility(View.GONE);
            spinner_toggle_arr2.setVisibility(View.VISIBLE);
            current_toggle_id = R.array.t2Arr;
        }else{
            spinner_toggle_arr1.setVisibility(View.VISIBLE);
            spinner_toggle_arr2.setVisibility(View.GONE);
            current_toggle_id = R.array.t1Arr;
        }

    }

    private void Initialize(){
        //fps monitor
        FPSlabel = (TextView) activity.findViewById(R.id.textViewFPS);

        //spinners
        AdapterView.OnItemSelectedListener onsel_listener = new AdapterView.OnItemSelectedListener(){
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                toggle_id = i;
                update_toggle_item_display(-1.0f);
                Pair cp =(Pair)ui_map.get(current_toggle_id)[toggle_id];
                JUIsetJavaUIStatus(0, (String)cp.first);
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        };
        spinner_toggle_arr1 = (Spinner)activity.findViewById(R.id.toggleSpinnerArr1);
        spinner_toggle_arr1.setOnItemSelectedListener(onsel_listener);

        spinner_toggle_arr2 = (Spinner)activity.findViewById(R.id.toggleSpinnerArr2);
        spinner_toggle_arr2.setOnItemSelectedListener(onsel_listener);

        spinner_toggle_sub = (Spinner)activity.findViewById(R.id.toggleSubSpinner);
        spinner_toggle_sub.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                toggle_id_sub = i;
                update_toggle_item_display(-1.0f);}
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {}
        });

        //switches
        spinner_switch = (Spinner)activity.findViewById(R.id.switchSpinner);
        spinner_switch.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                switch_id = i;
//                JUIsetJavaUIStatus(1, switch_id);
                update_switch_item_display();
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        switch_widget = (Switch)activity.findViewById(R.id.switcherSwitch);
        switch_widget.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(!compoundButton.isPressed()) return;
                Pair cp = (Pair)ui_map.get(R.array.switchArr)[switch_id];
                bool_values.put((String)cp.first, !bool_values.get((String)cp.first));
                update_switch_item_display();
            }
        });

        //toggles
        seekbar_top = (SeekBar) activity.findViewById(R.id.toggleSeekBar);
        toggleValueTex = (TextView) activity.findViewById(R.id.toggleText);
        seekbar_top.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                update_toggle_item_display((float)i);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        //bottom panel
        bottom_panel = (View)activity.findViewById(R.id.bottomPanel);
        bottom_panel.setVisibility(View.GONE);
        seekbar_bottom = (SeekBar)activity.findViewById(R.id.cuttingSeekBar);
        seekbar_bottom.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                JUIsetParam("cutting", 1.0f * i / toggle_max_map.get("cutting").second);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        updateSpinnerEnteries();
        update_switch_item_display();
        update_toggle_item_display(-1);
        Pair cp =(Pair)ui_map.get(current_toggle_id)[toggle_id];
        JUIsetJavaUIStatus(0, (String)cp.first);
    }
    private void update_toggle_item_display(float value) {
        Pair cp =(Pair)ui_map.get(current_toggle_id)[toggle_id], cp_sub = (Pair)ui_map.get(R.array.opacityArr)[toggle_id_sub];
        if(entry_has_sub_arrs.contains(cp.first)){//sub
            seekbar_top.setMax(toggle_max_sub_pair.second);
            if(value >= .0f)
                toggle_values_sub.put((String)cp_sub.first, value);
            float toggle_value = toggle_values_sub.get(cp_sub.first).floatValue();
            seekbar_top.setProgress((int)toggle_value);
            JUIsetParam((String) cp_sub.first, toggle_value * toggle_max_sub_pair.first / toggle_max_sub_pair.second);
            spinner_toggle_sub.setVisibility(View.VISIBLE);
        }else{
            spinner_toggle_sub.setVisibility(View.GONE);
            seekbar_top.setMax(toggle_max_map.get(cp.first).second);
            if(value >= .0f)
                toggle_values.put((String)cp.first, value);
            float toggle_value = toggle_values.get(cp.first).floatValue();
            seekbar_top.setProgress((int)toggle_value);
            JUIsetParam((String)cp.first, toggle_value * toggle_max_map.get(cp.first).first / toggle_max_map.get(cp.first).second);
        }
        seekbar_bottom.setMax(toggle_max_map.get("cutting").second);
        seekbar_bottom.setProgress((int)toggle_values.get("cutting").floatValue());
    }
    private void update_switch_item_display(){
        Pair cp = (Pair)ui_map.get(R.array.switchArr)[switch_id];
        boolean cvalue = bool_values.get(cp.first);
        if(cp.first == "cutting"){
            if(cvalue){
                bottom_panel.setVisibility(View.VISIBLE);
                Toast.makeText(activity, "Use Bottom SeekBar to Cut", Toast.LENGTH_LONG).show();
            }else
                bottom_panel.setVisibility(View.GONE);
            return;
        }
        updateSpinnerEnteries();
        switch_widget.setChecked(cvalue);
        JUIsetSwitches((String)cp.first, cvalue);
    }

    public static void ToggleShowView_animate(){
        View panel = activity.findViewById(R.id.topPanel);
        if(panel.getVisibility() == View.VISIBLE){
            panel.startAnimation(panelHiddenAction);
            panel.setVisibility(View.GONE);
        }else{
            panel.startAnimation(panelShownAction);
            panel.setVisibility(View.VISIBLE);
        }
    }
    public void updateFPS(){
        activity.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                FPSlabel.setText(String.format("%2.2f FPS", JUIgetFPS()));
            }});
    }

    public static native void JUIonSingleTouchDown(float x, float y);
    public static native void JUIonTouchMove(float x, float y);

    public static native void JUIsetJavaUIStatus(int item, String key);
    public static native void JUIsetParam(String key, float value);
    public static native void JUIsetSwitches(String key, boolean value);

    public static native float JUIgetFPS();
}

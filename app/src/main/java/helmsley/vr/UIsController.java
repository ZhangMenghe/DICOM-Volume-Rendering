package helmsley.vr;

import android.app.Activity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.AdapterView;

import android.widget.Button;
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

import helmsley.vr.DUIs.cuttingUIs;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.proto.fileTransferClient;

public class UIsController {
    public static Activity activity;
    final static String TAG = "UIsController";
    protected cuttingUIs cuttingController;
    protected dialogUIs dialogController;
    // UIs
    public static TextView FPSlabel, toggleValueTex;

    private Spinner spinner_toggle_arr1, // toggle Opacity, for texturebased
                    spinner_toggle_arr2, //toggle sample step and so on.. for raycast
                    spinner_toggle_sub, //toggle subset of opacity
                    spinner_switch;
    private SeekBar seekbar_top;
    private Switch switch_widget;

    private static boolean first_time=true;

    private View toggle_panel;
    private int toggle_id = 0, toggle_id_sub = 0, switch_id = 0;
    private Map<String, Float> toggle_values=new HashMap<>(), toggle_values_sub=new HashMap<>();
    private Map<String, Boolean> bool_values=new HashMap<>();

//    private List<float[]> tvalues = new ArrayList<float[]>();
    public static TranslateAnimation panelHiddenAction, panelShownAction;

    //array code -> key-value array
    //Important! Order matters!!Align with the resource file
    public static Map<Integer, Pair<String,?>[] > ui_map= new HashMap<Integer, Pair<String,?>[]>() {{
        put(R.array.t1Arr, new Pair[] {new Pair("Opacity", -1.0f)});
        put(R.array.t2Arr, new Pair[] {new Pair("samplestep", 400.0f), new Pair("threshold", 1.0f), new Pair("brightness", 600.0f)});
        put(R.array.opacityArr, new Pair[] {new Pair("overall", 1.0f), new Pair("lowbound", 1.0f), new Pair("cutoff", .0f)});
        put(R.array.switchArr, new Pair[] {new Pair("colortrans", false),new Pair("raycast", true), new Pair("cutting", false), new Pair("maskon", false), new Pair("accumulate", false)});
        put(R.array.bottomArr, new Pair[] {new Pair("cutting", .0f)});
    }};
    public static Map<String, Pair<Float, Integer>> toggle_max_map = new HashMap<String, Pair<Float, Integer>>(){{
        put("samplestep", new Pair(800.0f, 800)); put("threshold", new Pair(2.0f, 50)); put("brightness", new Pair(1500.0f, 500)); put("cutting", new Pair(1.0f, 50));
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

    public UIsController(final Activity activity_) {
        activity = activity_;
        dialogController = new dialogUIs(activity_);
        cuttingController = new cuttingUIs(activity_);
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
                        toggle_values.put((String)par.first, (Float)par.second / toggle_max_map.get(par.first).first *toggle_max_map.get(par.first).second);
                        JUIsetParam((String)par.first, (float)par.second);
                        break;
                    case R.array.opacityArr:
                        toggle_values_sub.put((String)par.first, (Float)par.second  / toggle_max_sub_pair.first * toggle_max_sub_pair.second);
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
    private boolean IsRaycast(){
        Pair<String, ?>[] arr = ui_map.get(R.array.switchArr);
        return bool_values.get((String) arr[1].first);
    }
    private void updateSpinnerEnteries(){
        if(IsRaycast()){
            spinner_toggle_arr1.setVisibility(View.GONE);
            spinner_toggle_sub.setVisibility(View.INVISIBLE);
            spinner_toggle_arr2.setVisibility(View.VISIBLE);
            current_toggle_id = R.array.t2Arr;
            update_toggle_item_display(-1);
        }else{
            spinner_toggle_arr1.setVisibility(View.VISIBLE);
            spinner_toggle_sub.setVisibility(View.VISIBLE);
            spinner_toggle_arr2.setVisibility(View.GONE);
            current_toggle_id = R.array.t1Arr;
            update_toggle_item_display(-1);
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

        toggle_panel = (View)activity.findViewById(R.id.togglePanel);


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
            float real_value = toggle_value * toggle_max_sub_pair.first / toggle_max_sub_pair.second;
            JUIsetParam((String) cp_sub.first, real_value);
            spinner_toggle_sub.setVisibility(View.VISIBLE);
            toggleValueTex.setText(activity.getString(R.string.text_toggle, real_value));
        }else{
            spinner_toggle_sub.setVisibility(View.GONE);
            seekbar_top.setMax(toggle_max_map.get(cp.first).second);
            if(value >= .0f)
                toggle_values.put((String)cp.first, value);
            float toggle_value = toggle_values.get(cp.first).floatValue();
            seekbar_top.setProgress((int)toggle_value);
            float real_value2 = toggle_value * toggle_max_map.get(cp.first).first / toggle_max_map.get(cp.first).second;
            JUIsetParam((String)cp.first, real_value2);
            toggleValueTex.setText(activity.getString(R.string.text_toggle, real_value2));
        }
       cuttingController.onSeekBarChange(IsRaycast(), toggle_values.get("cutting"));
    }
    private void update_switch_item_display(){
        Pair cp = (Pair)ui_map.get(R.array.switchArr)[switch_id];
        boolean cvalue = bool_values.get(cp.first);
        cuttingController.onCuttingStateChange(IsRaycast(), cp.first == "cutting"&&cvalue, toggle_panel);

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
    public static void debug_create_downloader(){

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
    public static native void JUIonScale(float sx, float sy);
    public static native void JUIonPan(float x, float y);

    public static native void JUIsetJavaUIStatus(int item, String key);
    public static native void JUIsetParam(String key, float value);
    public static native void JUIsetSwitches(String key, boolean value);

    public static native float JUIgetFPS();
}

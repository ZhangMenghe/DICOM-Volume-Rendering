package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;

import com.google.common.primitives.Floats;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class cutplaneUIs extends BasePanel{
    final static String TAG = "cutplaneUIs";

    //widgets
    private SeekBar seek_bar_;
    private Button btn_next, btn_prev;

    CheckBox traversal_check_box;
    private ctCheckboxListAdapter cbAdapter_;
    private final static int TRAVERSAL_CHECK_ID = 3;
    private static boolean default_traversal_check;
    private View primary_panel, traversal_panel;
    private final static float[]default_cut_pose={0,0,0,0,0,-1};
    private static float initial_progress;
    private static int max_progress_value;
    private Switch view_switch;

    private int cutting_status = 0;//0 for none, 1 for cutting, 2 for traversal
    private final static int VIEW_SWITCH_ID = 4;

    public cutplaneUIs(final Activity activity, ViewGroup parent_view){
        super(activity, parent_view);

        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.cutting_panel, parent_view, false);

        //setup spinner
        Spinner spinner_check =  (Spinner)panel_.findViewById(R.id.spinner_check_cutting_control);
        cbAdapter_ = new ctCheckboxListAdapter(activity);
        spinner_check.setAdapter(cbAdapter_);

        //setup button
        btn_next = panel_.findViewById(R.id.cut_next_button);
        btn_next.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                JUIInterface.JUIsetCuttingPlaneDelta(0, 1);
                return true;
            }
        });

        btn_prev = panel_.findViewById(R.id.cut_prev_button);
        btn_prev.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                JUIInterface.JUIsetCuttingPlaneDelta(0, -1);
                return true;
            }
        });
        Resources res = activity.getResources();

        String[] check_names = res.getStringArray(R.array.cut_check_params);
        String[] check_values = res.getStringArray(R.array.cut_check_values);

        String main_check_name = check_names[0];
        default_primary_check = Boolean.parseBoolean(check_values[0]);

        primary_checkbox = (CheckBox)panel_.findViewById(R.id.check_cutting_show);
        primary_checkbox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(main_check_name, isChecked);
                if(isChecked){
                    traversal_check_box.setChecked(false);
                    JUIInterface.JUISwitchCuttingPlane(0);
                    primary_panel.setVisibility(View.VISIBLE);
                    traversal_panel.setVisibility(View.INVISIBLE);
                }else{
                    primary_panel.setVisibility(View.INVISIBLE);
                }
            }
        });

        String default_traversal_name = check_names[TRAVERSAL_CHECK_ID];
        default_traversal_check = Boolean.parseBoolean(check_values[TRAVERSAL_CHECK_ID]);

        traversal_check_box = panel_.findViewById(R.id.check_traversal_show);
        traversal_check_box.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(default_traversal_name, isChecked);
                if(isChecked){
                    primary_checkbox.setChecked(false);
                    primary_panel.setVisibility(View.INVISIBLE);
                    JUIInterface.JUISwitchCuttingPlane(1);
                    traversal_panel.setVisibility(View.VISIBLE);
                    cbAdapter_.ResetAll();
                }else{
                    traversal_panel.setVisibility(View.INVISIBLE);
                }
            }
        });

        primary_panel = panel_.findViewById(R.id.layout_cutting_plane);
        traversal_panel = panel_.findViewById(R.id.layout_traversal_plane);
        reset_checkbox_and_panel();


        //setup seekbar
        seek_bar_ = (SeekBar)panel_.findViewById(R.id.cutting_seekbar);
        String params[] = activity.getResources().getStringArray(R.array.cutting_plane);
        initial_progress = Float.parseFloat(params[0]);
        max_progress_value = Integer.parseInt(params[1]);
        seek_bar_.setMax(max_progress_value);
        seek_bar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                JUIInterface.JUIsetCuttingPlane(1.0f * i / max_progress_value);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        //setup switch
        view_switch = (Switch)panel_.findViewById(R.id.toggle_view_switch);
        view_switch.setChecked(Boolean.parseBoolean(check_values[VIEW_SWITCH_ID]));
        view_switch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                JUIInterface.JUIsetChecks(check_names[VIEW_SWITCH_ID], isChecked);
            }
        });

        sub_panels_.add(panel_);
        setup_checks(R.array.cut_check_params, R.array.cut_check_values);
    }

    private void reset_checkbox_and_panel(){
        if(default_primary_check)cutting_status = 1;
        else if(default_traversal_check)cutting_status=2;

        primary_checkbox.setChecked(default_primary_check);
        traversal_check_box.setChecked(default_traversal_check);

        if(default_primary_check)primary_panel.setVisibility(View.VISIBLE);
        else primary_panel.setVisibility(View.INVISIBLE);
        if(default_traversal_check)traversal_panel.setVisibility(View.VISIBLE);
        else traversal_panel.setVisibility(View.INVISIBLE);
    }
    public void Reset(){
        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        seek_bar_.setProgress((int)(Float.valueOf(params[0]) * max_seek_value));
        reset_checkbox_and_panel();
        view_switch.setChecked(false);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap cutmap = (LinkedHashMap) map.getOrDefault("cutting plane", null);
        if(cutmap == null) return;

        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        double percent = (Double) cutmap.getOrDefault("percentage", Double.valueOf(params[0]));
        seek_bar_.setProgress((int)(percent * max_seek_value));

        //todo:jui send cutting plane status(pos/ori)
        boolean cut_status = (Boolean) cutmap.getOrDefault("status", default_primary_check);
        boolean freeze_volume = (Boolean) cutmap.getOrDefault("freeze volume", default_check_vales[1]);
        boolean freeze_plane = (Boolean) cutmap.getOrDefault("freeze plane", default_check_vales[2]);
        primary_checkbox.setChecked(cut_status);

        cbAdapter_.setValue(0, freeze_volume); cbAdapter_.setValue(1,freeze_plane);
        Collections.addAll(names, check_names_);
        values.add(cut_status);
        values.add(freeze_volume);values.add(freeze_plane);


        float[] cut_pose = default_cut_pose.clone();

        float[] pos = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("ppoint", new ArrayList<Float>()));
        if(pos.length == 3) System.arraycopy(pos, 0, cut_pose, 0, 3);
        float[] norm = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("pnorm", new ArrayList<Float>()));
        if(norm.length == 3) System.arraycopy(norm, 0, cut_pose, 3, 3);
        JUIInterface.JUIsetAllTuneParamById(2, cut_pose);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        float[] cpv = JUIInterface.JUIgetCuttingPlaneStatus();
        map.put("status", primary_checkbox.isChecked());
        map.put("freeze volume", cbAdapter_.getValue(0));
        map.put("freeze plane", cbAdapter_.getValue(1));
        map.put("percentage", (float)seek_bar_.getProgress() / Integer.parseInt(params[1]));

        map.put("ppoint", new ArrayList<Float>(Arrays.asList(cpv[0], cpv[1], cpv[2])));
        map.put("pnorm", new ArrayList<Float>(Arrays.asList(cpv[3], cpv[4], cpv[5])));
        return map;
    }
    public void showHidePanel(boolean show_panel, boolean isRaycast){
        super.showHidePanel(show_panel);
    }
    public void onTexRayChange(boolean isRaycast){
        showHidePanel(panel_visible, isRaycast);
    }
    private static class ctCheckboxListAdapter extends ListAdapter {
        List<Boolean> item_values;
        boolean is_cutting;
        ctCheckboxListAdapter(Context context) {
            super(context, context.getString(R.string.check_freeze_name));
            //setup values
            Resources res = context.getResources();
            item_names = Arrays.asList(res.getStringArray(R.array.cut_freeze_params));
            TypedArray check_values = res.obtainTypedArray(R.array.cut_check_values);
            is_cutting = check_values.getBoolean(0, false);
            item_values = new ArrayList<>();
            for (int i = 0; i < item_names.size(); i++) item_values.add(check_values.getBoolean(i+1, false));
            check_values.recycle();
        }
        void setValue(int id, boolean value){
            if(id < item_values.size() && item_values.get(id)!=value) {
                item_values.set(id, value);
                notifyDataSetChanged();
            }
        }
        void ResetAll(){
            boolean data_changed = false;
            for(int i=0; i<item_values.size();i++){
                if(item_values.get(i)){
                    item_values.set(i, false);
                    JUIInterface.JUIsetChecks(item_names.get(i),false);
                    data_changed = true;
                }
            }
            if(data_changed)notifyDataSetChanged();
        }
        boolean getValue(int id){return (id<item_values.size())?item_values.get(id):false;}
        public View getDropDownView(int position, View convertView, ViewGroup parent){
            ViewContentHolder holder;
            if (convertView == null) {
                holder = new ViewContentHolder();
                convertView = mInflater.inflate(R.layout.spinner_check_layout, null);
                holder.text_name = (TextView) convertView.findViewById(R.id.checkName);
                holder.checkBox = (CheckBox) convertView.findViewById(R.id.checkCheckBox);

                convertView.setTag(R.layout.spinner_check_layout, holder);

            } else {
                holder = (ViewContentHolder) convertView.getTag(R.layout.spinner_check_layout);
            }
            holder.text_name.setText(item_names.get(position));
            holder.checkBox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
                @Override
                public void onCheckedChanged(CompoundButton buttonView,
                                             boolean isChecked) {
                    if(item_values.get(position) == isChecked) return;
                    item_values.set(position, isChecked);
                    JUIInterface.JUIsetChecks(item_names.get(position), isChecked);

                    int relpos = 1-position;
                    if(isChecked && item_values.get(relpos)){
                        //update another one
                        JUIInterface.JUIsetChecks(item_names.get(relpos), false);
                        item_values.set(relpos, false);
                        notifyDataSetChanged();
                    }
                }
            });
            holder.checkBox.setTag(position);
            holder.checkBox.setChecked(item_values.get(position));
            return convertView;
        }
        class ViewContentHolder{
            TextView text_name;
            CheckBox checkBox;
        }
    }
}

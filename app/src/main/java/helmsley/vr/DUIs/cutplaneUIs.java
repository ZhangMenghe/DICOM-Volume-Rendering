package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.support.v4.content.ContextCompat;
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

public class cutplaneUIs extends BasePanel{
    final static String TAG = "cutplaneUIs";

    //widgets
    private SeekBar seek_bar_;
    private Button btn_next, btn_prev;

    CheckBox traversal_check_box;
    private ctCheckboxListAdapter cbAdapter_;
    private centerIdListAdapter cid_adapter;
    private static boolean default_traversal_check;
    private View primary_panel, traversal_panel;
    private static float initial_progress;
    private static int max_progress_value;
    private Switch view_switch;
    private ColorStateList normal_color, highlight_color;
    private boolean is_current_raycast;

    private final static int TRAVERSAL_CHECK_ID = 4;
    private final static int VIEW_SWITCH_ID = 5;
    private final static int TID_CUTTING_PLANE = 5;
    private final static float[]default_cut_pose={0,0,0,0,0,-1};

    public cutplaneUIs(final Activity activity, ViewGroup parent_view){
        super(activity, parent_view);
        Resources res = activity.getResources();

        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.cutting_panel, parent_view, false);

        //setup spinner
        Spinner spinner_check =  (Spinner)panel_.findViewById(R.id.spinner_check_cutting_control);
        cbAdapter_ = new ctCheckboxListAdapter(activity);
        spinner_check.setAdapter(cbAdapter_);

        //setup button
        highlight_color = ColorStateList.valueOf(ContextCompat.getColor(activity, R.color.yellowOrange));
        normal_color = ColorStateList.valueOf(ContextCompat.getColor(activity, R.color.brightBlue));

        btn_next = panel_.findViewById(R.id.cut_next_button);
        btn_next.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    btn_next.setBackgroundTintList(highlight_color);
                }else if(event.getAction()==MotionEvent.ACTION_UP){
                    btn_next.setBackgroundTintList(normal_color);
                }
                JUIInterface.JUIsetCuttingPlaneDelta(0, 1);
                return true;
            }
        });

        btn_prev = panel_.findViewById(R.id.cut_prev_button);
        btn_prev.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    btn_prev.setBackgroundTintList(highlight_color);//ColorStateList.valueOf(resources.getColor(R.id.blue_100)));
                }else if(event.getAction()==MotionEvent.ACTION_UP){
                    btn_prev.setBackgroundTintList(normal_color);
                }
                JUIInterface.JUIsetCuttingPlaneDelta(0, -1);
                return true;
            }
        });

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
                    if(is_current_raycast)primary_panel.setVisibility(View.VISIBLE);
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

        //setup traversal id
        Spinner cid_spinner = (Spinner)panel_.findViewById(R.id.centerline_id_spinner);
        cid_adapter = new centerIdListAdapter(activity);
        cid_spinner.setAdapter(cid_adapter);

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
        primary_checkbox.setChecked(default_primary_check);
        traversal_check_box.setChecked(default_traversal_check);

        if(default_primary_check)primary_panel.setVisibility(View.VISIBLE);
        else primary_panel.setVisibility(View.INVISIBLE);
        if(default_traversal_check)traversal_panel.setVisibility(View.VISIBLE);
        else traversal_panel.setVisibility(View.INVISIBLE);
    }
    public void Reset(){
        seek_bar_.setProgress((int)(initial_progress * max_progress_value));
        reset_checkbox_and_panel();
        view_switch.setChecked(false);
        cid_adapter.setTitleById(0);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap cutmap = (LinkedHashMap) map.getOrDefault("cutting plane", null);
        if(cutmap == null) return;

        String[] params = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.parseInt(params[1]);
        Float percent = (Float)cutmap.getOrDefault("percentage", Double.valueOf(params[0]));
        seek_bar_.setProgress((int)(percent * max_seek_value));

        //todo:jui send cutting plane status(pos/ori)
        boolean cut_status = (Boolean) cutmap.getOrDefault("status", default_primary_check);
        boolean freeze_volume = (Boolean) cutmap.getOrDefault("freeze volume", default_check_vales[1]);
        boolean freeze_plane = (Boolean) cutmap.getOrDefault("freeze plane", default_check_vales[2]);
        boolean real_sampled = (Boolean) cutmap.getOrDefault("real value", default_check_vales[3]);
        boolean center_line_traversal = (Boolean) cutmap.getOrDefault("Center Line Travel", default_traversal_check);
        boolean b_traversal_view = (Boolean) cutmap.getOrDefault("Traversal View", false);

        primary_checkbox.setChecked(cut_status);
        cbAdapter_.setValue(0, freeze_volume); cbAdapter_.setValue(1,freeze_plane);cbAdapter_.setValue(2,real_sampled);
        traversal_check_box.setChecked(center_line_traversal);
        view_switch.setChecked(b_traversal_view);
        cid_adapter.setTitleByText((String) cutmap.getOrDefault("traversal target", "Colon"));

        Collections.addAll(names, check_names_);
        values.add(cut_status);values.add(freeze_volume);values.add(freeze_plane);values.add(center_line_traversal);values.add(b_traversal_view);


        float[] cut_pose = default_cut_pose.clone();

        float[] pos = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("ppoint", new ArrayList<Float>()));
        if(pos.length == 3) System.arraycopy(pos, 0, cut_pose, 0, 3);
        float[] norm = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("pnorm", new ArrayList<Float>()));
        if(norm.length == 3) System.arraycopy(norm, 0, cut_pose, 3, 3);
        JUIInterface.JUIsetAllTuneParamById(TID_CUTTING_PLANE, cut_pose);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        String[] params = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        float[] cpv = JUIInterface.JUIgetCuttingPlaneStatus();
        map.put("status", primary_checkbox.isChecked());
        map.put("freeze volume", cbAdapter_.getValue(0));
        map.put("freeze plane", cbAdapter_.getValue(1));
        map.put("real value", cbAdapter_.getValue(2));
        map.put("Center Line Travel", traversal_check_box.isChecked());
        map.put("Traversal View", view_switch.isChecked());

        map.put("percentage", (float)seek_bar_.getProgress() / Integer.parseInt(params[1]));

        map.put("ppoint", new ArrayList<Float>(Arrays.asList(cpv[0], cpv[1], cpv[2])));
        map.put("pnorm", new ArrayList<Float>(Arrays.asList(cpv[3], cpv[4], cpv[5])));
        map.put("traversal target", cid_adapter.getTitle());

        return map;
    }
    public void showHidePanel(boolean show_panel, boolean isRaycast){
        super.showHidePanel(show_panel);
    }

    public void onRenderingMethodChange(int rm_id){
        boolean is_all_dir = (rm_id !=0 );
        showHidePanel(panel_visible, is_all_dir);
        is_current_raycast = is_all_dir;
        if(primary_checkbox.isChecked()){
            if(is_all_dir)primary_panel.setVisibility(View.VISIBLE);
            else primary_panel.setVisibility(View.INVISIBLE);
        }
    }
    private static class centerIdListAdapter extends textSimpleListAdapter{
        centerIdListAdapter(Context context){
            super(context, R.array.cutting_traversal_target);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            JUIInterface.JUIsetTraversalTarget(id);
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            JUIInterface.JUIsetTraversalTarget(item_names.indexOf(this.title));
        }
        void onItemClick(int position){
            JUIInterface.JUIsetTraversalTarget(item_names.indexOf(this.title));
        }
    }
    private static class ctCheckboxListAdapter extends ListAdapter {
        List<Boolean> item_values;
        boolean is_cutting;
        ctCheckboxListAdapter(Context context) {
            super(context, context.getString(R.string.check_raycast_cut_option_name));
            //setup values
            Resources res = context.getResources();
            item_names = Arrays.asList(res.getStringArray(R.array.cut_raycast_options));
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

                    if(position<2){
                    int relpos = 1-position;
                    if(isChecked && item_values.get(relpos)){
                        //update another one
                        JUIInterface.JUIsetChecks(item_names.get(relpos), false);
                        item_values.set(relpos, false);
                        notifyDataSetChanged();
                    }}
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

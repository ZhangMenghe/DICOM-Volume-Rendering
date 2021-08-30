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
    private Button btn_next, btn_prev;
    private CheckBox traversal_check_box;
    private SeekBar traversal_seek_bar;
    private Switch view_switch;
    private View cut_linear_layout, traversal_linear_layout;

    //adapters
    private ctCheckboxListAdapter opt_adapter_;
    private centerIdListAdapter cid_adapter;

    private final String traversal_check_name, view_switch_name;
    private final Boolean default_traversal_check_value, default_view_check_value;
    private final float default_inital_seekbar_progress;
    private final int default_max_seekbar;

    private ColorStateList normal_color, highlight_color;

    private final static int TID_CUTTING_PLANE = 5;
    private final static float[]default_cut_pose={0,0,0,0,0,-1};

    public cutplaneUIs(final Activity activity, ViewGroup parent_view){
        super(activity, parent_view);
        Resources res = activity.getResources();

        final LayoutInflater mInflater = LayoutInflater.from(activity);
        View panel_ = mInflater.inflate(R.layout.cutting_panel, parent_view, false);

        //setup layout
        cut_linear_layout = panel_.findViewById(R.id.cut_linear_layout);
        traversal_linear_layout = panel_.findViewById(R.id.traversal_linear_layout);

        String[] check_names = res.getStringArray(R.array.cut_check_params);
        String[] check_values = res.getStringArray(R.array.cut_check_values);

        //setup spinner
        Spinner spinner_opt =  (Spinner)panel_.findViewById(R.id.spinner_check_cutting_control);
        opt_adapter_ = new ctCheckboxListAdapter(activity);
        spinner_opt.setAdapter(opt_adapter_);

        //setup traversal id
        Spinner cid_spinner = (Spinner)panel_.findViewById(R.id.centerline_id_spinner);
        cid_adapter = new centerIdListAdapter(activity);
        cid_spinner.setAdapter(cid_adapter);

        //setup switch
        view_switch = (Switch)panel_.findViewById(R.id.toggle_view_switch);
        view_switch_name = res.getString(R.string.cutting_view_switch_name);
        int VIEW_SWITCH_ID = java.util.Arrays.asList(check_names).indexOf(view_switch_name);
        default_view_check_value = Boolean.parseBoolean(check_values[VIEW_SWITCH_ID]);
        view_switch.setChecked(Boolean.parseBoolean(check_values[VIEW_SWITCH_ID]));
        view_switch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                JUIInterface.JUIsetChecks(check_names[VIEW_SWITCH_ID], isChecked);
            }
        });

        //setup seekbar
        traversal_seek_bar = (SeekBar)panel_.findViewById(R.id.seekbar_traversal);
        String params[] = activity.getResources().getStringArray(R.array.cutting_plane);
        default_inital_seekbar_progress = Float.parseFloat(params[0]);
        default_max_seekbar = Integer.parseInt(params[1]);
        traversal_seek_bar.setMax(default_max_seekbar);
        traversal_seek_bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                JUIInterface.JUIsetCuttingPlane(1.0f * i / default_max_seekbar);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

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

        sub_panels_.add(panel_);
        setup_checks(
                panel_,
                R.array.cut_check_params, R.array.cut_check_values,
                R.id.check_cutting_show, 0);

        //primary checkbox
        primary_checkbox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(main_check_name, isChecked);
                if(isChecked){
                    cut_linear_layout.setVisibility(View.VISIBLE);

                    traversal_check_box.setChecked(false);
                    traversal_seek_bar.setVisibility(View.INVISIBLE);
                    traversal_linear_layout.setVisibility(View.INVISIBLE);
                    JUIInterface.JUISwitchCuttingPlane(0);
                }
            }
        });

        //traversal checkbox
        traversal_check_name = res.getString(R.string.cutting_traversal_name);
        int TRAVERSAL_CHECK_ID = java.util.Arrays.asList(check_names).indexOf(traversal_check_name);
        default_traversal_check_value = Boolean.parseBoolean(check_values[TRAVERSAL_CHECK_ID]);

        traversal_check_box = panel_.findViewById(R.id.check_traversal_show);
        traversal_check_box.setChecked(default_traversal_check_value);
        traversal_check_box.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(traversal_check_name, isChecked);
                if(isChecked){
                    primary_checkbox.setChecked(false);
                    cut_linear_layout.setVisibility(View.INVISIBLE);

                    traversal_seek_bar.setVisibility(View.VISIBLE);
                    traversal_linear_layout.setVisibility(View.VISIBLE);
                    JUIInterface.JUISwitchCuttingPlane(1);
                    opt_adapter_.ResetAll();
                }
            }
        });
        traversal_seek_bar.setVisibility(default_traversal_check_value?View.VISIBLE:View.INVISIBLE);
        cut_linear_layout.setVisibility(default_primary_check?View.VISIBLE:View.INVISIBLE);
        traversal_linear_layout.setVisibility(default_traversal_check_value?View.VISIBLE:View.INVISIBLE);
    }

    public void Reset(){
        traversal_seek_bar.setProgress((int)(default_inital_seekbar_progress * default_max_seekbar));
        traversal_check_box.setChecked(default_traversal_check_value);
        view_switch.setChecked(default_view_check_value);
        traversal_seek_bar.setVisibility(default_traversal_check_value?View.VISIBLE:View.INVISIBLE);
        cut_linear_layout.setVisibility(default_primary_check?View.VISIBLE:View.INVISIBLE);
        traversal_linear_layout.setVisibility(default_traversal_check_value?View.VISIBLE:View.INVISIBLE);
        opt_adapter_.ResetAll();
        cid_adapter.setTitleById(0);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap cutmap = (LinkedHashMap) map.getOrDefault("cutting plane", null);
        if(cutmap == null) return;
//      seekbar for explicit value
//        String[] params = actRef.get().getResources().getStringArray(R.array.cutting_plane);
//        int max_seek_value = Integer.parseInt(params[1]);
//        Float percent = (Float)cutmap.getOrDefault("percentage", Double.valueOf(params[0]));
//        seek_bar_.setProgress((int)(percent * max_seek_value));

        //todo:jui send cutting plane status(pos/ori)
        boolean cut_status = (Boolean) cutmap.getOrDefault("Status", default_primary_check);
        boolean freeze_volume = (Boolean) cutmap.getOrDefault("Freeze Volume", default_check_vales[1]);
        boolean show_plane = (Boolean) cutmap.getOrDefault("Show Plane", default_check_vales[2]);
        boolean real_sampled = (Boolean) cutmap.getOrDefault("Real Value", default_check_vales[3]);
        boolean traversal = (Boolean) cutmap.getOrDefault("Center Line Travel", default_check_vales[4]);
        boolean traversal_view = (Boolean) cutmap.getOrDefault("Traversal View", default_check_vales[5]);

        primary_checkbox.setChecked(cut_status);
        opt_adapter_.setValue(0, freeze_volume); opt_adapter_.setValue(1,show_plane);opt_adapter_.setValue(2,real_sampled);
        traversal_check_box.setChecked(traversal);
        view_switch.setChecked(traversal_view);
        cid_adapter.setTitleByText((String) cutmap.getOrDefault("Traversal Target", "Colon"));

        Collections.addAll(names, check_names_);
        values.add(cut_status);values.add(freeze_volume);values.add(show_plane);values.add(real_sampled);values.add(traversal);values.add(traversal_view);

        float[] cut_pose = default_cut_pose.clone();

        float[] pos = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("Plane Point", new ArrayList<Float>()));
        if(pos.length == 3) System.arraycopy(pos, 0, cut_pose, 0, 3);
        float[] norm = Floats.toArray((ArrayList<Float>)cutmap.getOrDefault("Plane Normal", new ArrayList<Float>()));
        if(norm.length == 3) System.arraycopy(norm, 0, cut_pose, 3, 3);
        JUIInterface.JUIsetAllTuneParamById(TID_CUTTING_PLANE, cut_pose);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
//        String[] params = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        float[] cpv = JUIInterface.JUIgetCuttingPlaneStatus();
        map.put("Status", primary_checkbox.isChecked());
        map.put("Freeze Volume", opt_adapter_.getValue(0));
//        map.put("freeze plane", cbAdapter_.getValue(1));
        map.put("Show Plane", opt_adapter_.getValue(1));
        map.put("Real Value", opt_adapter_.getValue(2));
        map.put("Center Line Travel", traversal_check_box.isChecked());
        map.put("Traversal View", view_switch.isChecked());

        map.put("Plane Point", new ArrayList<Float>(Arrays.asList(cpv[0], cpv[1], cpv[2])));
        map.put("Plane Normal", new ArrayList<Float>(Arrays.asList(cpv[3], cpv[4], cpv[5])));
        map.put("Traversal Target", cid_adapter.getTitle());

        return map;
    }
    public void showHidePanel(boolean show_panel){
        super.showHidePanel(show_panel);
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
        ctCheckboxListAdapter(Context context) {
            super(context, context.getString(R.string.check_raycast_cut_option_name));
            //setup values
            Resources res = context.getResources();
            item_names = Arrays.asList(res.getStringArray(R.array.cut_raycast_options));
            TypedArray check_values = res.obtainTypedArray(R.array.cut_check_values);
            item_values = new ArrayList<>();
            for (int i = 0; i < item_names.size(); i++) item_values.add(check_values.getBoolean(i+1, false));
            check_values.recycle();
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
        void setValue(int id, boolean value){
            if(item_values.get(id)!=value){
                item_values.set(id, value);
                JUIInterface.JUIsetChecks(item_names.get(id), value);
                notifyDataSetChanged();
            }
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

package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.support.design.widget.FloatingActionButton;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

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
    private FloatingActionButton button_;

    private ctCheckboxListAdapter cbAdapter_;

    public cutplaneUIs(final Activity activity, ViewGroup parent_view){
        super(activity, parent_view);

        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.cutting_panel, parent_view, false);

        //setup spinner
        Spinner spinner_check =  (Spinner)panel_.findViewById(R.id.spinner_check_cutting_control);
        cbAdapter_ = new ctCheckboxListAdapter(activity);
        spinner_check.setAdapter(cbAdapter_);

        //setup seekbar
        seek_bar_ = (SeekBar)panel_.findViewById(R.id.cutting_seekbar);
        String params[] = activity.getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        seek_bar_.setMax(max_seek_value);
        seek_bar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                JUIInterface.JUIsetCuttingPlane(UIsManager.tex_id, 1.0f * i / max_seek_value);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        //setup button
        button_ = panel_.findViewById(R.id.cutting_button);
        button_.setOnTouchListener(new View.OnTouchListener(){
            float down_v_x, down_e_x;
            float e_v_offset;
            public boolean onTouch(View view, MotionEvent event) {
                switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN:
                        down_v_x = view.getX();
                        down_e_x = event.getRawX();
                        e_v_offset = down_v_x - down_e_x;
                        break;

                    case MotionEvent.ACTION_MOVE:
                        JUIInterface.JUIsetCuttingPlane(UIsManager.raycast_id, event.getRawX() - down_e_x);
                        view.setX(e_v_offset + event.getRawX());
                        break;

                    case MotionEvent.ACTION_UP:
                        view.setX(down_v_x);
                        break;
                    case MotionEvent.ACTION_BUTTON_PRESS:

                    default:
                        return false;
                }
                return true;
            }

        });
        sub_panels_.add(panel_);

        setup_checks(
                panel_,
                R.array.cut_check_params, R.array.cut_check_values,
                R.id.check_cutting_show, 0);
    }
    public void Reset(){
        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        seek_bar_.setProgress((int)(Float.valueOf(params[0]) * max_seek_value));
        primary_checkbox.setChecked(default_primary_check);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap cutmap = (LinkedHashMap) map.getOrDefault("cutting plane", null);
        if(cutmap == null) return;

        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        double percent = (Double)cutmap.getOrDefault("percentage", Double.valueOf(params[0]));
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
    }

    public void showHidePanel(boolean show_panel, boolean isRaycast){
        super.showHidePanel(show_panel);

        if(isRaycast){
            button_.show();
            seek_bar_.setVisibility(View.INVISIBLE);
            if(panel_visible)Toast.makeText(actRef.get(), "Drag Bottom Button to Change", Toast.LENGTH_LONG).show();
        }else{
            button_.hide();
            seek_bar_.setVisibility(View.VISIBLE);
            if(panel_visible)Toast.makeText(actRef.get(), "Use Bottom SeekBar to Cut", Toast.LENGTH_LONG).show();
        }
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

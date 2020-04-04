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

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class cutplaneUIs {
    private final WeakReference<Activity> actRef;
    private final WeakReference<ViewGroup> parentRef;

    final static String TAG = "cutplaneUIs";

    //widgets
    private SeekBar seek_bar_;
    private FloatingActionButton button_;

    //panel
    final private View panel_;

    private ctCheckboxListAdapter cbAdapter_;
    private boolean panel_visible;

    public cutplaneUIs(final Activity activity, ViewGroup parent_view){
        actRef = new WeakReference<>(activity);
        parentRef = new WeakReference<>(parent_view);

        final LayoutInflater mInflater = LayoutInflater.from(activity);

        panel_ = mInflater.inflate(R.layout.cutting_panel, parent_view, false);
        panel_visible = false;

        //setup spinner
        Spinner spinner_check =  (Spinner)panel_.findViewById(R.id.spinner_check_cutting_control);
        cbAdapter_ = new ctCheckboxListAdapter(activity);
        spinner_check.setAdapter(cbAdapter_);

        //setup checkbox
        String check_cutting_show_str = activity.getResources().getString(R.string.cutting_check_name);
        CheckBox checkbox = (CheckBox)panel_.findViewById(R.id.check_cutting_show);
        checkbox.setChecked(cbAdapter_.getSeparateCheckBoxValue());
        checkbox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                JUIInterface.JUIsetChecks(check_cutting_show_str, isChecked);
            }
        });

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
        Reset();
    }
    public void Reset(){
        if(panel_visible){
            panel_visible = false;
            parentRef.get().removeView(panel_);
        }

        String params[] = actRef.get().getResources().getStringArray(R.array.cutting_plane);
        int max_seek_value = Integer.valueOf(params[1]);
        seek_bar_.setProgress((int)(Float.valueOf(params[0]) * max_seek_value));
    }
    public void onCuttingStateChange(boolean show_panel, boolean isRaycast){
        if(panel_visible && !show_panel) parentRef.get().removeView(panel_);
        else if(!panel_visible && show_panel) parentRef.get().addView(panel_);
        panel_visible = show_panel;

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
    public void onCuttingStateChange(boolean isRaycast){
        onCuttingStateChange(panel_visible, isRaycast);
    }
    private class ctCheckboxListAdapter extends ListAdapter {
        List<Boolean> item_values;
        boolean is_cutting;
        ctCheckboxListAdapter(Context context) {
            super(context, context.getString(R.string.check_freeze_name));
            //setup values
            Resources res = context.getResources();
            item_names = Arrays.asList(res.getStringArray(R.array.check_cutting_params));
            TypedArray check_values = res.obtainTypedArray(R.array.check_cutting_values);
            is_cutting = check_values.getBoolean(0, false);
            item_values = new ArrayList<>();
            for (int i = 0; i < item_names.size(); i++) item_values.add(check_values.getBoolean(i+1, false));
        }
        boolean getSeparateCheckBoxValue(){return is_cutting;}

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

package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.TypedArray;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class claheUIs extends BasePanel{
    private static WeakReference<claheUIs> selfRef;
    private final textSimpleListAdapter m_sel_adp;
    private final CLAHEVariableListAdapter m_var_adp;
    private final XYZListAdapter m_var_sub_adp;
    private final Spinner var_sub_spinner;
    private final TextView value_text;

    private final float[] DEFAULT_VALUES={0.85f, 4, 4, 2, 200, 200, 50, 0, 0, 0};
    private float[] m_values, m_values_new;
    private int m_current_var_pos, m_current_var_sub_pos;

    private final int[] DEFAULT_STEP_SIZE={20, 20, 10};
    private final float DEFAULT_CLIP_STEP = 0.05f;

    public claheUIs(final Activity activity, ViewGroup parent_view) {
        super(activity, parent_view);
        selfRef = new WeakReference<>(this);
        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.clahe_panel, parent_view, false);
        Spinner sel_spinner = (Spinner)panel_.findViewById(R.id.sel_clahe_spinner);
        m_sel_adp = new CLAHEListAdapter(activity, R.array.clahe_options);
        sel_spinner.setAdapter(m_sel_adp);

        Spinner var_spinner = (Spinner)panel_.findViewById(R.id.clahe_variable_spinner);
        var_sub_spinner = (Spinner)panel_.findViewById(R.id.clahe_variable_sub_spinner);

        m_var_adp = new CLAHEVariableListAdapter(activity, R.array.clahe_tune);
        var_spinner.setAdapter(m_var_adp);

        m_var_sub_adp = new XYZListAdapter(activity, R.array.xyz_dim);
        var_sub_spinner.setAdapter(m_var_sub_adp);

        ColorStateList highlight_color = ColorStateList.valueOf(ContextCompat.getColor(activity, R.color.yellowOrange));
        ColorStateList normal_color = ColorStateList.valueOf(ContextCompat.getColor(activity, R.color.brightBlue));

        Button btn_up = panel_.findViewById(R.id.clahe_up_btn);
        btn_up.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    btn_up.setBackgroundTintList(highlight_color);
                }else if(event.getAction()==MotionEvent.ACTION_UP){
                    if(m_current_var_pos == 0)m_values_new[0]+=DEFAULT_CLIP_STEP;
                    else if(m_current_var_pos == 1){m_values_new[1]+=1;m_values_new[2]+=1;m_values_new[3]+=1;}
                    else if(m_current_var_pos == 2){m_values_new[4+m_current_var_sub_pos]+=2*DEFAULT_STEP_SIZE[m_current_var_sub_pos];}
                    else{m_values_new[7+m_current_var_sub_pos]+=DEFAULT_STEP_SIZE[m_current_var_sub_pos];}

                    updateValueString();
                    JUIInterface.JUIsetCLAHEVariableDeltaStep(true, m_current_var_pos, m_current_var_sub_pos);
                    btn_up.setBackgroundTintList(normal_color);
                }
                return true;
            }
        });

        Button btn_prev = panel_.findViewById(R.id.clahe_drop_btn);
        btn_prev.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    btn_prev.setBackgroundTintList(highlight_color);//ColorStateList.valueOf(resources.getColor(R.id.blue_100)));
                }else if(event.getAction()==MotionEvent.ACTION_UP){
                    if(m_current_var_pos == 0)m_values_new[0]-=DEFAULT_CLIP_STEP;
                    else if(m_current_var_pos == 1){m_values_new[1]-=1;m_values_new[2]-=1;m_values_new[3]-=1;}
                    else if(m_current_var_pos == 2){m_values_new[4+m_current_var_sub_pos]-=2*DEFAULT_STEP_SIZE[m_current_var_sub_pos];}
                    else{m_values_new[7+m_current_var_sub_pos]-=DEFAULT_STEP_SIZE[m_current_var_sub_pos];}

                    updateValueString();
                    JUIInterface.JUIsetCLAHEVariableDeltaStep(false, m_current_var_pos, m_current_var_sub_pos);
                    btn_prev.setBackgroundTintList(normal_color);
                }
                return true;
            }
        });

        Button btn_apply = panel_.findViewById(R.id.clahe_apply_btn);
        btn_apply.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                m_values=m_values_new.clone();
                updateValueString();
                JUIInterface.JUIApplyCLAHEChanges();
            }
        });

        value_text = panel_.findViewById(R.id.clahe_variable_value);

        sub_panels_.add(panel_);
        setup_checks(
                panel_,
                R.array.clahe_check_params, R.array.clahe_check_values,
                R.id.check_clahe_enabled, 0);
    }

    @Override
    public void Reset(){
        m_values = DEFAULT_VALUES.clone();m_values_new=m_values.clone();

        m_sel_adp.setTitleById(0);
        m_var_adp.setTitleById(0);
        m_var_sub_adp.setTitleById(0);
        primary_checkbox.setChecked(default_primary_check);

        updateValueString();
    }
    private void updateValueString(){
        String prefix = m_var_adp.getTitle();
        float old_value, new_value;
        if(m_current_var_pos == 0){
            old_value = m_values[0]; new_value=m_values_new[0];
        }else{
            prefix+=" " + m_var_sub_adp.getTitle();
            int id = 1+(m_current_var_pos-1)*3 + m_current_var_sub_pos;
            old_value = m_values[id]; new_value=m_values_new[id];
        }

        String content = prefix+": "+String.valueOf(old_value);
        if(old_value != new_value) content+="->"+String.valueOf(new_value);
        value_text.setText(content);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap mmap = (LinkedHashMap) map.getOrDefault("clahe", null);
        if(mmap == null) return;
        boolean status = (Boolean) mmap.getOrDefault("Status", default_primary_check);
        primary_checkbox.setChecked(status);
        
        Collections.addAll(names, check_names_);
        values.add(status);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        map.put("Status", primary_checkbox.isChecked());
        return map;
    }
    @Override
    public void showHidePanel(boolean isPanelOn){
        super.showHidePanel(isPanelOn);
    }
    private class CLAHEListAdapter extends textSimpleListAdapter{
        CLAHEListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            JUIInterface.JUIsetCLAHEOption(id);
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            JUIInterface.JUIsetCLAHEOption(item_names.indexOf(this.title));
        }
        void onItemClick(int position){
            JUIInterface.JUIsetCLAHEOption(position);
        }
    }
    private class CLAHEVariableListAdapter extends textSimpleListAdapter{
        private boolean[] show_sub_arr;
        CLAHEVariableListAdapter(Context context, int arrayId){
            super(context, arrayId);
            show_sub_arr = new boolean[item_names.size()];
            TypedArray check_values = context.getResources().obtainTypedArray(R.array.clahe_tune_dimension);
            for (int i = 0; i < item_names.size(); i++) show_sub_arr[i] = check_values.getInt(i,0)>1;
            check_values.recycle();

            m_current_var_pos = 0;
            update_sub_spinner();
        }
        void update_sub_spinner(){
            var_sub_spinner.setVisibility(show_sub_arr[m_current_var_pos]?View.VISIBLE:View.INVISIBLE);
        }

        void setTitleById(int id){
            super.setTitleById(id);
            m_current_var_pos = id;
            updateValueString();
            update_sub_spinner();
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            m_current_var_pos = item_names.indexOf(title);
            updateValueString();

            update_sub_spinner();
        }
        void onItemClick(int position){
            m_current_var_pos = position;
            updateValueString();

            update_sub_spinner();
        }
    }
    private class XYZListAdapter extends textSimpleListAdapter{
        XYZListAdapter(Context context, int arrayId){
            super(context, arrayId);m_current_var_sub_pos=0;
        }
        void setTitleById(int id){
            super.setTitleById(id);m_current_var_sub_pos=id;
            updateValueString();
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);m_current_var_sub_pos = item_names.indexOf(title);
            updateValueString();
        }
        void onItemClick(int position){
            m_current_var_sub_pos = position;
            updateValueString();
        }
    }

}

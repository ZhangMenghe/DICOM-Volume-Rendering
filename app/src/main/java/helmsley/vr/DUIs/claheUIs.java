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

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class claheUIs extends BasePanel{
    private static WeakReference<claheUIs> selfRef;
    private final textSimpleListAdapter m_sel_adp;
    private final CLAHEVariableListAdapter m_var_adp;
    private final XYZListAdapter m_var_sub_adp;
    private final Spinner var_sub_spinner;
//    private final float DEFAULT_CLIP_3D = 0.85f;
//    private float clip_3d_value;
//
////        const glm::uvec3 DEFAULT_SUBLOCK_NUM = glm::uvec3(4, 4, 2);
////    const glm::uvec3 DEFAULT_BLOCK_SIZE = glm::uvec3(200, 200, 50);
////    const float DEFAULT_CLIP_3D = 0.85f;
////    const glm::uvec3 DEFAULT_STEP_SIZE[3]={
////        glm::uvec3(20,0,0),
////        glm::uvec3(0,20,0),
////        glm::uvec3(0,0,10),
////
////    };
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
                    btn_up.setBackgroundTintList(normal_color);
                }
                JUIInterface.JUIsetCLAHEVariableDeltaStep(true, m_var_adp.getCurrentPosition(), m_var_sub_adp.getCurrentPosition());
                return true;
            }
        });

        Button btn_prev = panel_.findViewById(R.id.clahe_drop_btn);
        btn_prev.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    btn_prev.setBackgroundTintList(highlight_color);//ColorStateList.valueOf(resources.getColor(R.id.blue_100)));
                }else if(event.getAction()==MotionEvent.ACTION_UP){
                    btn_prev.setBackgroundTintList(normal_color);
                }
                JUIInterface.JUIsetCLAHEVariableDeltaStep(false, m_var_adp.getCurrentPosition(), m_var_sub_adp.getCurrentPosition());
                return true;
            }
        });

        Button btn_apply = panel_.findViewById(R.id.clahe_apply_btn);
        btn_apply.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                JUIInterface.JUIApplyCLAHEChanges();
            }
        });

        sub_panels_.add(panel_);
        setup_checks(
                panel_,
                R.array.clahe_check_params, R.array.clahe_check_values,
                R.id.check_clahe_enabled, 0);
    }

    @Override
    public void Reset(){
        m_sel_adp.setTitleById(0);
        m_var_adp.setTitleById(0);
        m_var_sub_adp.setTitleById(0);
        primary_checkbox.setChecked(default_primary_check);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap mmap = (LinkedHashMap) map.getOrDefault("clahe", null);
        if(mmap == null) return;
        boolean status = (Boolean) mmap.getOrDefault("status", default_primary_check);
        primary_checkbox.setChecked(status);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        map.put("status", primary_checkbox.isChecked());
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
        int current_id;
        CLAHEVariableListAdapter(Context context, int arrayId){
            super(context, arrayId);
            show_sub_arr = new boolean[item_names.size()];
            TypedArray check_values = context.getResources().obtainTypedArray(R.array.clahe_tune_dimension);
            for (int i = 0; i < item_names.size(); i++) show_sub_arr[i] = check_values.getInt(i,0)>1;
            check_values.recycle();

            current_id = 0;
            update_sub_spinner();
        }
        void update_sub_spinner(){
            var_sub_spinner.setVisibility(show_sub_arr[current_id]?View.VISIBLE:View.INVISIBLE);
        }
        int getCurrentPosition(){return current_id;}
        void setTitleById(int id){
            super.setTitleById(id);
            current_id = id;
            update_sub_spinner();
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            current_id = item_names.indexOf(title);
            update_sub_spinner();
        }
        void onItemClick(int position){
            current_id = position;
            update_sub_spinner();
        }
    }
    private class XYZListAdapter extends textSimpleListAdapter{
        int current_id;

        XYZListAdapter(Context context, int arrayId){
            super(context, arrayId);current_id=0;
        }
        int getCurrentPosition(){return current_id;}
        void setTitleById(int id){
            super.setTitleById(id);current_id=id;
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);current_id = item_names.indexOf(title);
        }
        void onItemClick(int position){
            current_id = position;
        }
    }

}

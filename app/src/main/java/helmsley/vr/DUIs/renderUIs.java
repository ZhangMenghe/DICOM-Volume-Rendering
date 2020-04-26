package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.support.constraint.ConstraintLayout;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import com.google.common.primitives.Floats;
import com.google.common.primitives.Ints;
import com.yahoo.mobile.client.android.util.rangeseekbar.RangeSeekBar;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class renderUIs extends BasePanel{
    private final WeakReference<UIsManager> mUIManagerRef;
    private textSimpleListAdapter rendermodeAdapter, colorAdapter;
    private widgetListAdapter widAdapter;
    private tunerListAdapter rendertuneAdapter;
    final private static int RAYCAST_ID = 1;
    private static String CHECK_TEXRAY_NAME, CHECK_OVERLAYS;

    public renderUIs(final Activity activity, UIsManager manager, ViewGroup parent_view){
        super(activity, parent_view);
        mUIManagerRef = new WeakReference<>(manager);
        //panels
        final LayoutInflater mInflater = LayoutInflater.from(activity);
        View tune_panel_ = mInflater.inflate(R.layout.tune_panel, parent_view, false);
        View trans_panel_ = mInflater.inflate(R.layout.transfunc_panel, parent_view, false);
        View control_panel_ = mInflater.inflate(R.layout.render_control_panel, parent_view, false);

        //details of tune panel
        Spinner seekbar_spinner = (Spinner) tune_panel_.findViewById(R.id.tune_seekbar_spinner);
        seekbar_spinner.setDropDownVerticalOffset(150);
        tunerListAdapter tunerAdapter = new tunerListAdapter(activity, 0, R.string.opacity_group_name, R.array.opaParams);
        seekbar_spinner.setAdapter(tunerAdapter);

        Spinner contrast_seekbar_spinner = (Spinner) tune_panel_.findViewById(R.id.contrast_seekbar_spinner);
        contrast_seekbar_spinner.setDropDownVerticalOffset(150);
        rendertuneAdapter = new tunerListAdapter(activity,1, R.string.contrast_group_name, R.array.contrastParams);
        contrast_seekbar_spinner.setAdapter(rendertuneAdapter);

        Spinner widget_spinner = (Spinner)tune_panel_.findViewById(R.id.tune_widget_id_spinner);
        widAdapter = new widgetListAdapter(activity, new tunerListAdapter[]{tunerAdapter});
        widget_spinner.setAdapter(widAdapter);

        Button btn_add = (Button)tune_panel_.findViewById(R.id.tune_widget_add_button);
        btn_add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                widAdapter.addItem();
            }
        });
        Button btn_delete = (Button)tune_panel_.findViewById(R.id.tune_widget_delete_button);
        btn_delete.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                widAdapter.deleteItem();
            }
        });

        //details of render panel
        Spinner rm_spinner = (Spinner)control_panel_.findViewById(R.id.render_mode_spinner);
        rendermodeAdapter = new renderListAdapter(activity, R.array.rendering_mode);
        //get default rendering mode
        rm_spinner.setAdapter(rendermodeAdapter);

        Spinner color_spinner = (Spinner)control_panel_.findViewById(R.id.render_color_spinner);
        colorAdapter = new colorListAdapter(activity, R.array.color_schemes);
        color_spinner.setAdapter(colorAdapter);

        sub_panels_.add(tune_panel_);
        sub_panels_.add(trans_panel_);
        sub_panels_.add(control_panel_);

        View trans_constraint = trans_panel_.findViewById(R.id.trans_func_panel);
        View trans_graph_panel = trans_panel_.findViewById(R.id.trans_graph_panel);
        View trans_gray_panel = trans_panel_.findViewById(R.id.trans_gray_panel);
        View trans_color_mix_panel = trans_panel_.findViewById(R.id.trans_color_mix_panel);
        View trans_colorscheme_panel = trans_panel_.findViewById(R.id.trans_color_tune_panel);

        trans_graph_panel.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)control_panel_.getLayoutParams();
                params.setMargins(0, 0, 0, trans_constraint.getHeight());
                control_panel_.setLayoutParams(params);
                control_panel_.requestLayout();

                int[]location = new int[2];
                trans_graph_panel.getLocationInWindow(location);
                JUIInterface.JuisetGraphRect(0,trans_graph_panel.getWidth(),trans_graph_panel.getHeight(),location[0],location[1]);
                trans_graph_panel.setVisibility(View.INVISIBLE);
                trans_gray_panel.setVisibility(View.INVISIBLE);
                trans_color_mix_panel.setVisibility(View.INVISIBLE);
                trans_colorscheme_panel.getLocationInWindow(location);
                trans_colorscheme_panel.setVisibility(View.INVISIBLE);
                int height = trans_colorscheme_panel.getHeight() + trans_gray_panel.getHeight()+trans_color_mix_panel.getHeight();
                JUIInterface.JuisetGraphRect(1,trans_colorscheme_panel.getWidth(),height,location[0],location[1]);

                trans_graph_panel.getViewTreeObserver().removeOnGlobalLayoutListener(this);
            }
        });

        setup_checks(R.array.render_check_params, R.array.render_check_values);
        CHECK_TEXRAY_NAME = check_names_[0];//res.getStringArray(R.array.render_check_params)[0];
        CHECK_OVERLAYS = check_names_[1];//res.getStringArray(R.array.render_check_params)[1];
    }
    public void Reset(){
        Resources res = actRef.get().getResources();
        widAdapter.Reset();
        rendertuneAdapter.Reset();
        rendertuneAdapter.addInstance(0);
        //render mode should be the first to set!!
        int rm_id = Integer.parseInt(res.getString(R.string.default_render_mode_id));
        rendermodeAdapter.setTitleById(rm_id);
        int color_id = Integer.parseInt(res.getString(R.string.default_color_mode_id));
        colorAdapter.setTitleById(color_id);
        JUIInterface.JUIResetValues(rendertuneAdapter.TID, rendertuneAdapter.getDefaultValues());
    }
    private void onTexRaySwitch(boolean isRaycast){
        JUIInterface.JUIsetChecks(CHECK_TEXRAY_NAME, isRaycast);
    }
    public void showHidePanel(boolean show_panel){
        if(panel_visible && !show_panel){
            for(View v:sub_panels_)parentRef.get().removeView(v);
            JUIInterface.JUIsetChecks(CHECK_OVERLAYS, false);
        } else if(!panel_visible && show_panel){
            for(View v:sub_panels_)parentRef.get().addView(v);
            JUIInterface.JUIsetChecks(CHECK_OVERLAYS, true);
        }
        panel_visible = show_panel;
    }
//    private static class contrastListAdapter extends ListAdapter{
//        private int dropview_width;
//        private int current_wid;
//        private float[] value_min, value_max;
//        private float[] default_left_values, default_right_values;
//        private ArrayList<float[]> item_value_left, item_value_right;
//
//        contrastListAdapter(Context context){
//            super(context, context.getString(R.string.contrast_group_name));
//            dropview_width = (int)(Resources.getSystem().getDisplayMetrics().widthPixels *Float.parseFloat(context.getString(R.string.cf_drop_tune_w)) );
//            //setup values
//            Resources res = context.getResources();
//            TypedArray typed_params = res.obtainTypedArray(R.array.contrastParams);
//            int item_numbers = typed_params.length();
//            value_max = new float[item_numbers];value_min = new float[item_numbers];
//            default_left_values = new float[item_numbers];default_right_values = new float[item_numbers];
//            item_value_left = new ArrayList<>();item_value_right = new ArrayList<>();
//            item_names = new ArrayList<>();
//            for (int i = 0; i < item_numbers; i++) {
//                int resId = typed_params.getResourceId(i, -1);
//                if (resId == -1) continue;
//                String[] param = res.getStringArray(resId);
//                item_names.add(param[0]);
//                value_min[i] = Float.parseFloat(param[1]);value_max[i] = Float.parseFloat(param[2]);
//                default_left_values[i] = Float.parseFloat(param[3]);default_right_values[i] = Float.parseFloat(param[4]);
//            }
//            typed_params.recycle();
//        }
//        public void Reset(){
//            item_value_left.clear();item_value_right.clear();
//        }
//        void addInstance(int nid){
//            item_value_left.add(default_left_values.clone());
//            item_value_right.add(default_right_values.clone());
//            current_wid = nid;
//            notifyDataSetChanged();
//        }
//        void removeInstance(int id, int nid){
//            if(id>=item_value_left.size()) return;
//            item_value_left.remove(id); item_value_right.remove(id);
//            current_wid = nid;
//            notifyDataSetChanged();
//        }
//        void setCurrentId(int id){
//            current_wid = id;
//            notifyDataSetChanged();
//        }
//        float[] getDefaultLeftValues(){return default_left_values.clone();}
//        float[] getDefaultRightValues(){return default_right_values.clone();}
//        public View getDropDownView(int position, View convertView, ViewGroup parent) {
//            ViewHolder holder;
//            if (convertView == null) {
//                holder = new ViewHolder();
//                convertView = mInflater.inflate(R.layout.spinner_dualseekbar, null);
//                holder.text_name = (TextView) convertView.findViewById(R.id.tuneName);
//                holder.text_value = (TextView) convertView.findViewById(R.id.tuneValue);
//
//                holder.seekbar = convertView.findViewById(R.id.rangeSeekbar);
//
//                holder.seekbar.setRangeValues(value_min[position], value_max[position]);
//                holder.seekbar.setOnRangeSeekBarChangeListener(new RangeSeekBar.OnRangeSeekBarChangeListener<Float>() {
//                    @Override
//                    public void onRangeSeekBarValuesChanged(RangeSeekBar<?> bar, Float minValue, Float maxValue){
//                        if(minValue == item_value_left.get(current_wid)[position] && maxValue==item_value_right.get(current_wid)[position]) return;
//                        item_value_left.get(current_wid)[position] = minValue;
//                        item_value_right.get(current_wid)[position] = maxValue;
//                        holder.text_value.setText(contexRef.get().getString(R.string.tune_value_dual, minValue, maxValue));
//                        JUIInterface.JUIsetDualParamById(position, minValue, maxValue);
//                    }
//                });
//
//                holder.seekbar.setTag(position);
//                convertView.setMinimumWidth(dropview_width);
//                convertView.setTag(R.layout.spinner_tune_layout, holder);
//            } else {
//                holder = (ViewHolder)convertView.getTag(R.layout.spinner_tune_layout);
//            }
//            //set progress
//            float value_left = item_value_left.get(current_wid)[position];
//            float value_right = item_value_right.get(current_wid)[position];
//            holder.seekbar.setSelectedMaxValue(value_right);
//            holder.seekbar.setSelectedMinValue(value_left);
//
////            holder.seekbar.setProgress((int)(item_values.get(current_wid)[position] / item_value_max[position] * item_seek_max[position]));
//            holder.text_name.setText(item_names.get(position));
////            float value = item_values.get(current_wid)[position];
//            holder.text_value.setText(contexRef.get().getString(R.string.tune_value_dual, value_left, value_right));
//
//            return convertView;
//        }
//        static class ViewHolder {
//            TextView text_name;
//            TextView text_value;
//            RangeSeekBar<Float> seekbar;
//        }
//    }
    private static class tunerListAdapter extends ListAdapter{
        public int TID;
        private int dropview_width;

        private float[] item_value_max;
        private int[] item_seek_max;
        private float[] default_values;
        private ArrayList<float[]> item_values;
        private int current_wid;

        tunerListAdapter(Context context, int tid, int title_resid, int content_resid) {
            super(context, context.getString(title_resid));
            TID = tid;
            dropview_width = (int)(Resources.getSystem().getDisplayMetrics().widthPixels *Float.parseFloat(context.getString(R.string.cf_drop_tune_w)) );

            //setup values
            Resources res = context.getResources();
            TypedArray typed_params = res.obtainTypedArray(content_resid);

            int item_numbers = typed_params.length();
            item_value_max = new float[item_numbers];
            item_seek_max = new int[item_numbers];
            default_values = new float[item_numbers];
            item_values = new ArrayList<>();
            item_names = new ArrayList<>();

            for (int i = 0; i < item_numbers; i++) {
                int resId = typed_params.getResourceId(i, -1);
                if (resId == -1) continue;
                String[] param = res.getStringArray(resId);
                item_names.add(param[0]);
                default_values[i] = Float.parseFloat(param[1]);
                item_value_max[i] = Float.parseFloat(param[2]);
                item_seek_max[i] = Integer.parseInt(param[3]);
            }

            typed_params.recycle();
        }
        public void Reset(){
            item_values.clear();
        }
        void addInstance(int nid){
            item_values.add(default_values.clone());
            current_wid = nid;
            notifyDataSetChanged();
        }
        void removeInstance(int id, int nid){
            if(id>=item_values.size()) return;
            item_values.remove(id); current_wid = nid;
            notifyDataSetChanged();
        }
        void setCurrentId(int id){
            current_wid = id;
            notifyDataSetChanged();
        }
        float[] getDefaultValues(){return default_values.clone();}

        @Override
        public View getDropDownView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;
            if (convertView == null) {
                holder = new ViewHolder();
                convertView = mInflater.inflate(R.layout.spinner_tune_layout, null);
                holder.text_name = (TextView) convertView.findViewById(R.id.tuneName);
                holder.text_value = (TextView) convertView.findViewById(R.id.tuneValue);

                holder.seekbar = (SeekBar) convertView.findViewById(R.id.tuneSeekbar);
                holder.seekbar.setMax(item_seek_max[position]);
                holder.seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                        if(!b) return;
                        float value = (float)i / item_seek_max[position] * item_value_max[position];
                        if(value == item_values.get(current_wid)[position]) return;
                        item_values.get(current_wid)[position] = value;
                        //todo:lowest!!
                        if(item_names.get(position).equals("Lowest")) value*=item_values.get(current_wid)[position - 1];
                        holder.text_value.setText(contexRef.get().getString(R.string.tune_value, value));
                        JUIInterface.JUIsetTuneParamById(TID, position, value);
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                });
                holder.seekbar.setTag(position);
                convertView.setMinimumWidth(dropview_width);
                convertView.setTag(R.layout.spinner_tune_layout, holder);
            } else {
                holder = (ViewHolder)convertView.getTag(R.layout.spinner_tune_layout);
            }
            //set
            holder.seekbar.setProgress((int)(item_values.get(current_wid)[position] / item_value_max[position] * item_seek_max[position]));
            holder.text_name.setText(item_names.get(position));
            float value = item_values.get(current_wid)[position];
            holder.text_value.setText(contexRef.get().getString(R.string.tune_value, value));

            return convertView;
        }
        static class ViewHolder {
            TextView text_name;
            TextView text_value;
            SeekBar seekbar;
        }
    }
    private static class widgetListAdapter extends textSimpleListAdapter {
        private final ArrayList<WeakReference<tunerListAdapter>> mTunerRefs;

        private final static int INIT_ID = 0;
        int current_id;
        int widget_num;
        String name_prefix;
        private float[] jui_default_values = null;
        private int[] jui_default_lengths = null;

        widgetListAdapter(Context context, tunerListAdapter[] tuner_adapters) {
            super(context, new ArrayList<>());
            mTunerRefs = new ArrayList<>();
            for(tunerListAdapter adp:tuner_adapters)
                mTunerRefs.add(new WeakReference<>(adp));
            widget_num = 0;
            name_prefix = context.getString(R.string.tune_widget_name_prefix) + " ";
        }
        public void Reset(){
            if(widget_num > 0){
                JUIInterface.JUIremoveAllTuneWidget();
                for(int i=0; i<mTunerRefs.size();i++) mTunerRefs.get(i).get().Reset();
                widget_num = 0;
            }
            current_id = INIT_ID;
            title = name_prefix + INIT_ID;
            addItem();
        }
        void setTitleById(int id){
            if(id >= widget_num) return;

            current_id = id;
            title = name_prefix + id;
            for(int i=0; i<mTunerRefs.size();i++) mTunerRefs.get(i).get().setCurrentId(id);

            JUIInterface.JUIsetTuneWidgetById(id);
            notifyDataSetChanged();
        }
        public int getCount(){return widget_num;}
        public void addItem(){
            if(widget_num >= 5) return;//todo:set maxmum of widget num

            widget_num++;
            current_id = widget_num-1;
            title = name_prefix + current_id;

            //change relevant stuff
            if(jui_default_lengths == null){
                jui_default_lengths = new int[mTunerRefs.size()];
                ArrayList<Float> values = new ArrayList<>();
                for(int i=0; i<mTunerRefs.size();i++){

                    float[] tvalues = mTunerRefs.get(i).get().getDefaultValues();
                    jui_default_lengths[i] = tvalues.length;
                    for(float v:tvalues) values.add(v);
                }
                jui_default_values = Floats.toArray(values);
            }
            for(int i=0; i<mTunerRefs.size();i++) mTunerRefs.get(i).get().addInstance(current_id);
            JUIInterface.JUIAddTuneParams(jui_default_lengths, jui_default_values);

//            JUIInterface.JUIsetDualParamById(0, lvalues[0], rvalues[0]);
            JUIInterface.JUIsetTuneWidgetById(current_id);

            notifyDataSetChanged();
        }
        void deleteItem(){
            deleteItem(current_id);
        }
        public void deleteItem(int id){
            if(widget_num<2 || id >= widget_num) return;

            widget_num--;
            current_id = id%widget_num;
            title = name_prefix + current_id;

            //change relevant stuff
            for(int i=0; i<mTunerRefs.size();i++) mTunerRefs.get(i).get().removeInstance(id,current_id);
            JUIInterface.JUIremoveTuneWidgetById(id);
            JUIInterface.JUIsetTuneWidgetById(current_id);

            notifyDataSetChanged();
        }
        void onItemClick(int position){
            setTitleById(position);
        }
        void onItemClick(String text_title){
            setTitleById(Integer.parseInt(text_title.substring(text_title.length()-1)));
        }
        public View getDropDownView(int position, View convertView, ViewGroup parent) {
            return super.getViewWithText(convertView, name_prefix + position, position);
        }
    }
    private class renderListAdapter extends textSimpleListAdapter{
        //TODO: SET THIS!

        int current_id = -1;
        renderListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void onItemClick(int position){
//            if(position == current_id) return;
            mUIManagerRef.get().onTexRaySwitch(position == RAYCAST_ID);
            onTexRaySwitch(RAYCAST_ID == position);
            current_id = position;
        }

        void setTitleById(int id){
//            if(id == current_id) return;
            super.setTitleById(id);
            mUIManagerRef.get().onTexRaySwitch(RAYCAST_ID == id);
            onTexRaySwitch(RAYCAST_ID == id);
            current_id = id;
        }
        int getRenderingModeById(){return current_id;}
    }
    private class colorListAdapter extends textSimpleListAdapter{
        colorListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            JUIInterface.JuisetColorScheme(id);
        }
        void onItemClick(int position){
            JUIInterface.JuisetColorScheme(position);
        }
    }
}

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

import java.lang.ref.WeakReference;
import java.util.ArrayList;


import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class renderUIs extends BasePanel{
    private final WeakReference<UIsManager> mUIManagerRef;
    private tunerListAdapter tunerAdapter;
    private textSimpleListAdapter rendermodeAdapter, colorAdapter;
    private widgetListAdapter widAdapter;
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
        tunerAdapter = new tunerListAdapter(activity);
        seekbar_spinner.setAdapter(tunerAdapter);

        Spinner widget_spinner = (Spinner)tune_panel_.findViewById(R.id.tune_widget_id_spinner);
        widAdapter = new widgetListAdapter(activity, tunerAdapter);
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
//                Log.e("TAG", "=====color scheme: "+location[0] + "  " + location[1] );


//                trans_gray_panel.getLocationInWindow(location);
//                JUIInterface.JuisetGraphRect(1,trans_gray_panel.getWidth(),trans_gray_panel.getHeight(),location[0],location[1]);
                trans_gray_panel.setVisibility(View.INVISIBLE);
//                Log.e("TAG", "=====color scheme: "+location[0] + "  " + location[1] );

//                trans_color_mix_panel.getLocationInWindow(location);
//                JUIInterface.JuisetGraphRect(2,trans_color_mix_panel.getWidth(),trans_color_mix_panel.getHeight(),location[0],location[1]);
                trans_color_mix_panel.setVisibility(View.INVISIBLE);
//                Log.e("TAG", "=====color scheme: "+location[0] + "  " + location[1] );


                trans_colorscheme_panel.getLocationInWindow(location);
//                JUIInterface.JuisetGraphRect(3,trans_colorscheme_panel.getWidth(),trans_colorscheme_panel.getHeight(),location[0],location[1]);
                trans_colorscheme_panel.setVisibility(View.INVISIBLE);
//                Log.e("TAG", "=====color scheme: "+location[0] + "  " + location[1] );

//                Log.e("TAG", "=====color scheme: "+schemeloc[0] + "  " + schemeloc[1] );
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
        tunerAdapter.Reset();
        //render mode should be the first to set!!
        int rm_id = Integer.parseInt(res.getString(R.string.default_render_mode_id));
        rendermodeAdapter.setTitleById(rm_id);
        int color_id = Integer.parseInt(res.getString(R.string.default_color_mode_id));
        colorAdapter.setTitleById(color_id);
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
    private static class tunerListAdapter extends ListAdapter{
        private float[] item_value_max;
        private int[] item_seek_max;
        private int dropview_width;
        private float[] default_values;
        private float[] item_values;
        tunerListAdapter(Context context) {
            super(context, context.getString(R.string.tune_group_name));
            dropview_width = (int)(Resources.getSystem().getDisplayMetrics().widthPixels *Float.parseFloat(context.getString(R.string.cf_drop_tune_w)) );

            //setup values
            Resources res = context.getResources();
            TypedArray typed_params = res.obtainTypedArray(R.array.opaParams);

            int item_numbers = typed_params.length();
            item_value_max = new float[item_numbers];
            item_seek_max = new int[item_numbers];
            default_values = new float[item_numbers];
            item_values = new float[item_numbers];
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
            Reset();
        }
        public void Reset(){
            item_values = default_values.clone();
        }
        float[] getDefaultValues(){
            return default_values.clone();
        }
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
                        float value = (float)i / item_seek_max[position] * item_value_max[position];
                        if(value == item_values[position]) return;
                        item_values[position] = value;
                        //todo:lowest!!
                        if(item_names.get(position).equals("Lowest")) value*=item_values[position - 1];
                        holder.text_value.setText(contexRef.get().getString(R.string.tune_value, value));
                        JUIInterface.JUIsetTuneParamById(position, value);
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                });
                holder.seekbar.setTag(position);
                holder.seekbar.setProgress((int)(item_values[position] / item_value_max[position] * item_seek_max[position]));
                convertView.setMinimumWidth(dropview_width);
                convertView.setTag(R.layout.spinner_tune_layout, holder);
            } else {
                holder = (ViewHolder)convertView.getTag(R.layout.spinner_tune_layout);
            }

            holder.text_name.setText(item_names.get(position));
            float value = item_values[position];
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
        private final WeakReference<tunerListAdapter> mTunerRef;
        private final static int INIT_ID = 0;
        int current_id;
        int widget_num;
        String name_prefix;

        widgetListAdapter(Context context, tunerListAdapter tuner_adapter) {
            super(context, new ArrayList<>());
            mTunerRef = new WeakReference<>(tuner_adapter);
            current_id = INIT_ID;
            widget_num = 0;
            name_prefix = context.getString(R.string.tune_widget_name_prefix) + " ";
            title = name_prefix + INIT_ID;
        }
        public void Reset(){
            JUIInterface.JUIremoveAllTuneWidget();
            current_id = INIT_ID;
            widget_num = 0;
            title = name_prefix + INIT_ID;
            addItem();
        }
        void setTitleById(int id){
            if(id < widget_num){
                current_id = id;
                title = name_prefix + id;
                JUIInterface.JUIsetTuneWidgetById(id);
                notifyDataSetChanged();
            }
        }
        public int getCount(){return widget_num;}
        public void addItem(){
            float[] values = mTunerRef.get().getDefaultValues();
            JUIInterface.JUIAddTuneParams(values.length, values);
            widget_num++;
            setTitleById(widget_num-1);

            notifyDataSetChanged();
        }
        void deleteItem(){
            deleteItem(current_id);
        }
        public void deleteItem(int id){
            if(widget_num<2 || id >= widget_num) return;
            JUIInterface.JUIremoveTuneWidgetById(id);
            widget_num--;
            setTitleById(id%widget_num);

            notifyDataSetChanged();
        }
        void onItemClick(int position){
            setTitleById(position);
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

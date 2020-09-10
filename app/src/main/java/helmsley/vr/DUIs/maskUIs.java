package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.TextView;

import com.google.common.primitives.Booleans;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.R;

public class maskUIs extends BasePanel{
    private RecyclerView recyclerView;
    private maskRecyclerViewAdapter recyclerViewAdapter;
    private maskCheckboxListAdapter cbAdapter_;
    final String[] mask_on_off_keys = {"status", "recolor", "volume", "mesh", "wireframe","center line" };
    public maskUIs(final Activity activity, ViewGroup parent_view) {
        super(activity, parent_view);
        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.mask_panel, parent_view, false);
        sub_panels_.add(panel_);
        recyclerView = (RecyclerView)panel_.findViewById(R.id.mask_recycle);
        recyclerView.setHasFixedSize(true);
        LinearLayoutManager HorizontalLayout = new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false);
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        recyclerView.setLayoutManager(layout_manager);
        recyclerView.setLayoutManager(HorizontalLayout);

        recyclerViewAdapter = new maskRecyclerViewAdapter(activity, recyclerView);
        recyclerView.setAdapter(recyclerViewAdapter);

        //setup spinner
        Spinner spinner_check = (Spinner)panel_.findViewById(R.id.spinner_check_mask_control);
        cbAdapter_ = new maskCheckboxListAdapter(activity);
        spinner_check.setAdapter(cbAdapter_);

        setup_checks(R.array.mask_check_params, R.array.mask_check_values);
    }
    @Override
    public void Reset(){
        recyclerViewAdapter.Reset();
        cbAdapter_.Reset(actRef.get());
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap maskmap = (LinkedHashMap) map.getOrDefault("mask", null);
        if(maskmap == null) return;
        int idx = 0;
        for(String key:mask_on_off_keys){
            boolean status = (Boolean) maskmap.getOrDefault(key, default_primary_check);
            cbAdapter_.setValue(idx++, status);
            values.add(status);
        }

        recyclerViewAdapter.Reset(Booleans.toArray((ArrayList<Boolean>)maskmap.getOrDefault("value", null)));
        Collections.addAll(names, check_names_);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        int idx = 0;
        for(String key:mask_on_off_keys)
            map.put(key, cbAdapter_.getValue(idx++));

        ArrayList<Boolean> values= new ArrayList<Boolean>();
        for(boolean b:recyclerViewAdapter.getValues()) values.add(b);
        map.put("value", values);
        return map;
    }
    @Override
    public void showHidePanel(boolean isPanelOn){
        super.showHidePanel(isPanelOn);
    }
    private static class maskCheckboxListAdapter extends ListAdapter {
        List<Boolean> item_values;
        maskCheckboxListAdapter(Context context) {
            super(context, context.getString(R.string.mask_check_name));
            //setup values
            item_names = Arrays.asList(context.getResources().getStringArray(R.array.mask_check_params));
            item_values = new ArrayList<>();
            Reset(context);
        }
        public void Reset(Context context){
            item_values.clear();
            TypedArray check_values = context.getResources().obtainTypedArray(R.array.mask_check_values);
            for (int i = 0; i < item_names.size(); i++) item_values.add(check_values.getBoolean(i, false));
            check_values.recycle();
        }
        void setValue(int id, boolean value){
            if(id < item_values.size() && item_values.get(id)!=value) {
                item_values.set(id, value);
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
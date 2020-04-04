package helmsley.vr.DUIs;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class checkboxAdapter {
    checkboxListAdapter mAdapter;
    final WeakReference<Context> contexRef;
    private final WeakReference<UIsManager> mUIManagerRef;

    static LinkedHashMap<String, Boolean> check_map;
    private static String mask_name, raycast_name, cutting_name, freeze_plane_name, freeze_volume_name;
    public checkboxAdapter(Context context){
        contexRef = new WeakReference<>(context);
        mUIManagerRef = null;
        Reset();
    }
    public checkboxAdapter(Context context, UIsManager manager){
        contexRef = new WeakReference<>(context);
        mUIManagerRef = new WeakReference<>(manager);
        Reset();
    }
    protected void setupCheckMapValue(Resources res) {
        //setup check map values
        String check_items[] = res.getStringArray(R.array.checkParams);
        TypedArray check_values = res.obtainTypedArray(R.array.checkValues);
        boolean values[] = new boolean[check_items.length];

        for(int i=0; i<check_items.length; i++){
            values[i] = check_values.getBoolean(i, false);
            check_map.put(check_items[i], values[i]);
        }
        mask_name = res.getString(R.string.mask_check_name);
        raycast_name = res.getString(R.string.texray_check_name);
        cutting_name = res.getString(R.string.cutting_check_name);
        freeze_plane_name=res.getString(R.string.freezeplane_check_name);
        freeze_volume_name = res.getString(R.string.freezeVolume_check_name);

        mUIManagerRef.get().onTexRaySwitch(check_map.get(raycast_name));
        mUIManagerRef.get().onMaskPanelSwitch(check_map.get(mask_name));
        JUIInterface.JUIInitCheckParam(check_items.length, check_items, values);
    }
    public void Reset(){
        check_map=new LinkedHashMap<>();
        //setup initial values
        setupCheckMapValue(contexRef.get().getResources());
        mAdapter = null;
    }
    public checkboxListAdapter getListAdapter(){
        if(mAdapter == null){
            mAdapter = new checkboxListAdapter(
                    contexRef.get(),
                    new ArrayList<>(check_map.keySet()),
                    new ArrayList<>(check_map.values()),
                    contexRef.get().getString(R.string.check_group_name));
        }
        return mAdapter;
    }

    public class checkboxListAdapter extends ListAdapter{
        ArrayList<Boolean> item_values;

        checkboxListAdapter(Context context,
                            ArrayList<String> item_names, ArrayList<Boolean> item_values,
                            String title) {
            super(context, title);
            this.item_names = item_names;
            this.item_values = item_values;
        }
        void addItem(String name, boolean value){
            if(this.item_names.contains(name)) return;
            this.item_names.add(name);
            this.item_values.add(value);
            notifyDataSetChanged();
        }
        void removeItem(String name){
            int id = this.item_names.indexOf(name);
            if(id == -1) return;
            this.item_names.remove(id);
            this.item_values.remove(id);
            notifyDataSetChanged();
        }
        boolean getItemValue(String name){
            int position = this.item_names.indexOf(name);
            return position==-1? false:this.item_values.get(position);
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
                    String item_name = item_names.get(position);

                    if(item_name.equals(raycast_name)) onTexRaySwitch(isChecked);
                    else if(item_name.equals(mask_name)) onMaskPanelSwitch(isChecked);
                    else if(item_name.equals(cutting_name)) onCuttingPlaneSwitch(isChecked);
                    else if(item_name.equals(freeze_plane_name)) onFreezePlaneSwitch(isChecked);
                    else if(item_name.equals(freeze_volume_name)) onFreezeVolumeSwitch(isChecked);

                    item_values.set(position, isChecked);
                    updateValue(item_name, isChecked);
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
    private void onTexRaySwitch(boolean isChecked){
        if(isChecked && mAdapter.getItemValue(cutting_name)) mAdapter.addItem(freeze_plane_name, false);
        else mAdapter.removeItem(freeze_plane_name);

        mUIManagerRef.get().onTexRaySwitch(isChecked);
    }
    private void onMaskPanelSwitch(boolean isChecked){
        mUIManagerRef.get().onMaskPanelSwitch(isChecked);
    }
    private void onCuttingPlaneSwitch(boolean isChecked){
        if(isChecked && mAdapter.getItemValue(raycast_name)) mAdapter.addItem(freeze_plane_name, false);
        else mAdapter.removeItem(freeze_plane_name);

        mUIManagerRef.get().onCuttingPlaneSwitch(isChecked);

        if(check_map.get(mask_name))onMaskPanelSwitch(false);

    }
    private void onFreezePlaneSwitch(boolean isChecked){
//        cutplaneUIs.isPlaneFreeze = isChecked;
        //mutual exclusion true
        if(isChecked && mAdapter.getItemValue(freeze_volume_name)){
            int position = mAdapter.item_names.indexOf(freeze_volume_name);
            mAdapter.item_values.set(position, false);
            JUIInterface.JUIsetChecks(mAdapter.item_names.get(position), false);
            mAdapter.notifyDataSetChanged();
        }
    }
    private void onFreezeVolumeSwitch(boolean isChecked){
        //mutual exclusion true
        if(isChecked && mAdapter.getItemValue(freeze_plane_name)){
//            cutplaneUIs.isPlaneFreeze = false;
            int position = mAdapter.item_names.indexOf(freeze_plane_name);
            mAdapter.item_values.set(position, false);
            JUIInterface.JUIsetChecks(mAdapter.item_names.get(position), false);
            mAdapter.notifyDataSetChanged();
        }
    }
    private void updateValue(String item_name, boolean isChecked){
        check_map.put(item_name, isChecked);
    }
}
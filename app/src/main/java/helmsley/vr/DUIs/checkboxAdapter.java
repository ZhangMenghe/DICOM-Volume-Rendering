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

public class checkboxAdapter {
    private WeakReference<checkboxListAdapter> mAdapterRef = null;
    private final WeakReference<Context> contexRef;

    private LinkedHashMap<String, Boolean> check_map=new LinkedHashMap<>();

    public checkboxAdapter(Context context){
        contexRef = new WeakReference<>(context);
        //setup initial values
        setupCheckMapValue(contexRef.get().getResources());
    }
    private void setupCheckMapValue(Resources res) {
        //setup check map values
        String check_items[] = res.getStringArray(R.array.checkParams);
        TypedArray check_values = res.obtainTypedArray(R.array.checkValues);
        boolean values[] = new boolean[check_items.length];

        for(int i=0; i<check_items.length; i++){
            values[i] = check_values.getBoolean(i, false);
            check_map.put(check_items[i], values[i]);
        }
        JUIInterface.JUIInitCheckParam(check_items.length,check_items,values);
    }
    public checkboxListAdapter createListAdapter(int index){
        if(mAdapterRef == null){
            checkboxListAdapter adapter = new checkboxListAdapter(
                    contexRef.get(),
                    new ArrayList<>(check_map.keySet()),
                    new ArrayList<>(check_map.values()),
                    contexRef.get().getString(R.string.check_group_name));
            mAdapterRef = new WeakReference<>(adapter);
        }
        return mAdapterRef.get();
    }
    public class checkboxListAdapter extends ListAdapter{
        private ArrayList<Boolean> item_values;

        public checkboxListAdapter(Context context,
                           ArrayList<String> item_names, ArrayList<Boolean> item_values,
                           String title) {
            super(context, title);
            this.item_names = item_names;
            this.item_values = item_values;
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
                    item_values.set(position, isChecked);
                    //todo:jnis
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
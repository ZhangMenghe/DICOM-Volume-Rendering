package helmsley.vr.DUIs;

import android.content.Context;
import android.content.res.TypedArray;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.Arrays;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class checkpanelAdapter extends ListAdapter {
    final WeakReference<Context> contexRef;
    private final WeakReference<UIsManager> mUIManagerRef;
    private boolean[] item_values;

    checkpanelAdapter(Context context, UIsManager manager) {
        super(context, context.getString(R.string.check_panel_group_name));
        contexRef = new WeakReference<>(context);
        mUIManagerRef = new WeakReference<>(manager);
        item_names = Arrays.asList(context.getResources().getStringArray(R.array.checkShowPanelName));
        item_values = new boolean[item_names.size()];
    }
    public void Reset(){
        TypedArray check_values_type = contexRef.get().getResources().obtainTypedArray(R.array.checkShowPanelValues);
        for(int i=0; i<item_values.length; i++)
            item_values[i] = check_values_type.getBoolean(i, false);
        check_values_type.recycle();
    }
    public boolean[] getAllValues(){return item_values;}

    public View getDropDownView(int position, View convertView, ViewGroup parent) {
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
        holder.checkBox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if (item_values[position] == isChecked) return;
                item_values[position]= isChecked;
                switch (position){
                    case 0:
                        onRenderingSwitch(isChecked);
                        break;
                    case 1:
                        onCuttingPlaneSwitch(isChecked);
                        break;
                    case 2:
                        onMaskPanelSwitch(isChecked);
                        break;
                    default:
                        break;
                }

            }
        });
        holder.checkBox.setTag(position);
        holder.checkBox.setChecked(item_values[position]);
        return convertView;
    }

    class ViewContentHolder {
        TextView text_name;
        CheckBox checkBox;
    }

    private void onCuttingPlaneSwitch(boolean isChecked) {
        mUIManagerRef.get().onCuttingPlaneSwitch(isChecked);
    }

    private void onRenderingSwitch(boolean isChecked){
        mUIManagerRef.get().onRenderingSwitch(isChecked);
    }

    private void onMaskPanelSwitch(boolean isChecked){
        mUIManagerRef.get().onMaskPanelSwitch(isChecked);
    }
}

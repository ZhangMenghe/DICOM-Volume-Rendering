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
import java.util.Arrays;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class checkpanelAdapter extends ListAdapter {
    final WeakReference<Context> contexRef;
    private final WeakReference<UIsManager> mUIManagerRef;
    private static String cutting_name, rendering_name, mask_name;
    List<Boolean> item_values;

    public checkpanelAdapter(Context context, UIsManager manager) {
        super(context, context.getString(R.string.check_panel_group_name));

        contexRef = new WeakReference<>(context);
        mUIManagerRef = new WeakReference<>(manager);
        Reset();
    }
    public void Reset(){
        Resources res = contexRef.get().getResources();
        item_names = Arrays.asList(res.getStringArray(R.array.checkShowPanelName));
        TypedArray check_values = res.obtainTypedArray(R.array.checkShowPanelValues);
        item_values = new ArrayList<>();
        for (int i = 0; i < item_names.size(); i++)
            item_values.add(check_values.getBoolean(i, false));
        cutting_name = res.getString(R.string.cutting_check_name);
        rendering_name = res.getString(R.string.panel_rendering_name);
        mask_name = res.getString(R.string.panel_mask_name);
        check_values.recycle();;
    }

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
                if (item_values.get(position) == isChecked) return;
                item_values.set(position, isChecked);
                String item_name = item_names.get(position);
                if(item_name.equals(cutting_name)) onCuttingPlaneSwitch(isChecked);
                else if(item_name.equals(rendering_name)) onRenderingSwitch(isChecked);
                else if(item_name.equals(mask_name)) onMaskPanelSwitch(isChecked);
            }
        });
        holder.checkBox.setTag(position);
        holder.checkBox.setChecked(item_values.get(position));
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

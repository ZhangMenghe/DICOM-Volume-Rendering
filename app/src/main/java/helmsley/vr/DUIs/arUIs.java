package helmsley.vr.DUIs;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class arUIs extends BasePanel{
    public arUIs(final Activity activity, ViewGroup parent_view) {
        super(activity, parent_view);
        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.ar_panel, parent_view, false);
        sub_panels_.add(panel_);

        setup_checks(
                panel_,
                R.array.ar_check_params, R.array.ar_check_values,
                R.id.check_ar_enabled, 0);
    }
    @Override
    public void Reset(){
        primary_checkbox.setChecked(default_primary_check);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap maskmap = (LinkedHashMap) map.getOrDefault("mask", null);
        if(maskmap == null) return;
        boolean status = (Boolean) maskmap.getOrDefault("status", default_primary_check);
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
}

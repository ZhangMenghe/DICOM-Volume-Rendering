package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class claheUIs extends BasePanel{
    private static WeakReference<claheUIs> selfRef;
    private textSimpleListAdapter m_sel_adp;


    public claheUIs(final Activity activity, ViewGroup parent_view) {
        super(activity, parent_view);
        selfRef = new WeakReference<>(this);
        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.clahe_panel, parent_view, false);
        Spinner sel_spinner =  (Spinner)panel_.findViewById(R.id.sel_clahe_spinner);
        m_sel_adp = new CLAHEListAdapter(activity, R.array.clahe_options);
        sel_spinner.setAdapter(m_sel_adp);

        sub_panels_.add(panel_);
        setup_checks(
                panel_,
                R.array.clahe_check_params, R.array.clahe_check_values,
                R.id.check_clahe_enabled, 0);
    }

    @Override
    public void Reset(){
        m_sel_adp.setTitleById(0);
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
}

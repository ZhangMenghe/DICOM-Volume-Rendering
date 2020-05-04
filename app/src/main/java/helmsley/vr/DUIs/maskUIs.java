package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.google.common.primitives.Booleans;

import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class maskUIs extends BasePanel{
    private RecyclerView recyclerView;
    private maskRecyclerViewAdapter recyclerViewAdapter;

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

        setup_checks(
                panel_,
                R.array.mask_check_params, R.array.mask_check_values,
                R.id.check_mask_show, 0);
    }
    @Override
    public void Reset(){
        recyclerViewAdapter.Reset();
        primary_checkbox.setChecked(default_primary_check);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap maskmap = (LinkedHashMap) map.getOrDefault("mask", null);
        if(maskmap == null) return;
        boolean status = (Boolean) maskmap.getOrDefault("status", default_primary_check);
        primary_checkbox.setChecked(status);
        recyclerViewAdapter.Reset(Booleans.toArray((ArrayList<Boolean>)maskmap.getOrDefault("value", null)));
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        map.put("status", primary_checkbox.isChecked());
        map.put("value", recyclerViewAdapter.getValues());
        return map;
    }
    @Override
    public void showHidePanel(boolean isPanelOn){
        super.showHidePanel(isPanelOn);
    }
}

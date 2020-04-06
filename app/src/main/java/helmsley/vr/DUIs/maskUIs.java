package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
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
    }
    @Override
    public void showHidePanel(boolean isPanelOn){
        super.showHidePanel(isPanelOn);
    }
}

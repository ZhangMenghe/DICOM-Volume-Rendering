package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;

import helmsley.vr.R;

public class maskUIs {
    private View panel;
    RecyclerView recyclerView;
    maskRecyclerViewAdapter recyclerViewAdapter;

    public maskUIs(final Activity activity) {
        panel = (View)activity.findViewById(R.id.maskPanel);

        recyclerView = (RecyclerView)activity.findViewById(R.id.mask_recycle);
        recyclerView.setHasFixedSize(true);
        LinearLayoutManager HorizontalLayout = new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false);
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        recyclerView.setLayoutManager(layout_manager);
        recyclerView.setLayoutManager(HorizontalLayout);


        recyclerViewAdapter = new maskRecyclerViewAdapter(activity, recyclerView);
        recyclerView.setAdapter(recyclerViewAdapter);

    }
    public void onStateChange(boolean isPanelOn){
//        if(isPanelOn) panel.setVisibility(View.VISIBLE);
//        else panel.setVisibility(View.INVISIBLE);
        DUIHelpers.ToggleShowView_animate(panel, isPanelOn);
    }
    public void Reset(){
        recyclerViewAdapter.Reset();
    }

}

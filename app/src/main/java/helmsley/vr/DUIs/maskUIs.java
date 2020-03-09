package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.Button;

import helmsley.vr.R;

public class maskUIs {
    private View panel;
    private Button hide_btn, show_btn;

    private boolean is_panel_on;
    View hideBottomPanel;

    RecyclerView recyclerView;
    maskRecyclerViewAdapter recyclerViewAdapter;

    public maskUIs(final Activity activity) {
        panel = (View)activity.findViewById(R.id.maskPanel);
        hideBottomPanel = (View)activity.findViewById(R.id.hiddenBottomPanel);

        hide_btn = (Button)activity.findViewById(R.id.mask_hide_button);
        show_btn = (Button)activity.findViewById(R.id.show_button);
        View.OnClickListener show_hide_listener = new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onStateChange(!is_panel_on, true);
//                if(!is_panel_on) hideBottomPanel.setVisibility(View.VISIBLE);
            }
        };
        hide_btn.setOnClickListener(show_hide_listener);
        show_btn.setOnClickListener(show_hide_listener);

        recyclerView = (RecyclerView)activity.findViewById(R.id.mask_recycle);
        recyclerView.setHasFixedSize(true);
        LinearLayoutManager HorizontalLayout = new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false);
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        recyclerView.setLayoutManager(layout_manager);
        recyclerView.setLayoutManager(HorizontalLayout);


        recyclerViewAdapter = new maskRecyclerViewAdapter(activity, recyclerView);
        recyclerView.setAdapter(recyclerViewAdapter);

    }
    public void onStateChange(boolean isPanelOn, boolean keepHideIcon){
        is_panel_on = isPanelOn;
        if(hideBottomPanel.getVisibility() == View.VISIBLE && !isPanelOn){
            panel.setVisibility(View.INVISIBLE);
        }else{
            DUIHelpers.ToggleShowView_animate(panel, isPanelOn);
        }
        if(keepHideIcon && !is_panel_on) hideBottomPanel.setVisibility(View.VISIBLE);
        else hideBottomPanel.setVisibility(View.INVISIBLE);
    }
    public void Reset(){
        recyclerViewAdapter.Reset();
    }
}

package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import java.lang.ref.WeakReference;

import helmsley.vr.R;

public class maskUIs {
    private final WeakReference<ViewGroup> parentRef;

    //panel
    final private View panel_;

    RecyclerView recyclerView;
    maskRecyclerViewAdapter recyclerViewAdapter;
    private boolean panel_visible;

    public maskUIs(final Activity activity, ViewGroup parent_view) {
        parentRef = new WeakReference<>(parent_view);

        final LayoutInflater mInflater = LayoutInflater.from(activity);

        panel_ = mInflater.inflate(R.layout.mask_panel, parent_view, false);
        panel_visible = false;

        recyclerView = (RecyclerView)panel_.findViewById(R.id.mask_recycle);
        recyclerView.setHasFixedSize(true);
        LinearLayoutManager HorizontalLayout = new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false);
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        recyclerView.setLayoutManager(layout_manager);
        recyclerView.setLayoutManager(HorizontalLayout);

        recyclerViewAdapter = new maskRecyclerViewAdapter(activity, recyclerView);
        recyclerView.setAdapter(recyclerViewAdapter);
    }
    public void Reset(){
        if(panel_visible){
            panel_visible = false;
            parentRef.get().removeView(panel_);
        }
        recyclerViewAdapter.Reset();
    }
    public boolean isMaskOn(){
        //todo:
        return false;
    }
    public void showHidePanel(boolean isPanelOn){
        if(panel_visible && !isPanelOn) parentRef.get().removeView(panel_);
        else if(!panel_visible && isPanelOn) parentRef.get().addView(panel_);
        panel_visible = isPanelOn;
    }
}

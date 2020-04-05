package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class renderUIs {
    private final WeakReference<ViewGroup> parentRef;
    private final WeakReference<UIsManager> mUIManagerRef;
    //panel
    final private View tune_panel_;
    final private View control_panel_;

    private Spinner seekbar_spinner;
    private SeekbarAdapter seekbarAdapter = null;
    private textSimpleListAdapter rendermodeAdapter, colorAdapter;
    private boolean panel_visible;

    public renderUIs(final Activity activity, UIsManager manager, ViewGroup parent_view){
        mUIManagerRef = new WeakReference<>(manager);
        parentRef = new WeakReference<>(parent_view);

        //panels
        final LayoutInflater mInflater = LayoutInflater.from(activity);
        tune_panel_ = mInflater.inflate(R.layout.tune_panel, parent_view, false);
        control_panel_ = mInflater.inflate(R.layout.transfunc_panel, parent_view, false);
        panel_visible = false;

        //details of tune panel
        Spinner widget_spinner = (Spinner)tune_panel_.findViewById(R.id.tune_widget_id_spinner);
        widget_spinner.setAdapter(new widgetListAdapter(activity));

        seekbar_spinner = (Spinner)tune_panel_.findViewById(R.id.tune_seekbar_spinner);
        seekbarAdapter = new SeekbarAdapter(activity);

        Button btn_add = (Button)tune_panel_.findViewById(R.id.tune_widget_add_button);
        Button btn_delete = (Button)tune_panel_.findViewById(R.id.tune_widget_delete_button);

        //details of render panel
        Spinner rm_spinner = (Spinner)control_panel_.findViewById(R.id.render_mode_spinner);
        rendermodeAdapter = new renderListAdapter(activity, R.array.rendering_mode);
        //get default rendering mode
        String drm = activity.getResources().getString(R.string.default_render_mode);
        rendermodeAdapter.setTitleByText(drm);
        rm_spinner.setAdapter(rendermodeAdapter);
//        onTexRaySwitch(drm.equals(activity.getResources().getString(R.string.texray_check_name)));

        Spinner color_spinner = (Spinner)control_panel_.findViewById(R.id.render_color_spinner);
        colorAdapter = new textSimpleListAdapter(activity, R.array.color_schemes);
        colorAdapter.setTitleByText(activity.getResources().getString(R.string.default_color_mode));
        color_spinner.setAdapter(colorAdapter);

        Reset();
//        mUIManagerRef.get().onTexRaySwitch(drm.equals(activity.getResources().getString(R.string.texray_check_name)));
    }
    public void Reset(){
        if(panel_visible){
            panel_visible = false;
            parentRef.get().removeView(tune_panel_);parentRef.get().removeView(control_panel_);
        }

    }
    public void onTexRaySwitch(boolean isRaycast){
        seekbar_spinner.setAdapter(seekbarAdapter.getListAdapter(isRaycast?1:0));
    }
    public void showHidePanel(boolean show_panel){
        if(panel_visible && !show_panel) {parentRef.get().removeView(tune_panel_);parentRef.get().removeView(control_panel_);}
        else if(!panel_visible && show_panel) {parentRef.get().addView(tune_panel_);parentRef.get().addView(control_panel_);}
        panel_visible = show_panel;
    }
    private class widgetListAdapter extends ListAdapter {
        int current_id = 0;
        int widget_num = 1;

        widgetListAdapter(Context context) {
            super(context, context.getString(R.string.tune_widget_id_name, 0));
            current_id = 0;
            item_names = new ArrayList<>();
            for (int i = 0; i < widget_num; i++)
                item_names.add(context.getString(R.string.tune_widget_id_name, i));
        }
    }
    private class renderListAdapter extends textSimpleListAdapter{
        //TODO: SET THIS!
        int RAYCAST_ID = 0;
        renderListAdapter(Context context, int arrayId){
            super(context, arrayId);
            mUIManagerRef.get().onTexRaySwitch(RAYCAST_ID == 0);
            onTexRaySwitch(RAYCAST_ID == 0);
        }
        void onItemClick(int position){
            mUIManagerRef.get().onTexRaySwitch(position == RAYCAST_ID);
            onTexRaySwitch(RAYCAST_ID == position);
        }
    }
    private class colorListAdapter extends textSimpleListAdapter{
        colorListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void onItemClick(int position){
//            JUIInterface.JuisetColorScheme(position);
        }
    }
}

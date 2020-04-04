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

public class renderUIs {
    private final WeakReference<ViewGroup> parentRef;
    //panel
    final private View tune_panel_;
    final private View control_panel_;

    private Spinner seekbar_spinner;
    private SeekbarAdapter seekbarAdapter = null;
    private textSimpleListAdapter rendermodeAdapter, colorAdapter;
    private boolean panel_visible;

    public renderUIs(final Activity activity, ViewGroup parent_view){
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
        seekbar_spinner.setAdapter(seekbarAdapter.getListAdapter(1));

        Button btn_add = (Button)tune_panel_.findViewById(R.id.tune_widget_add_button);
        Button btn_delete = (Button)tune_panel_.findViewById(R.id.tune_widget_delete_button);

        //details of render panel
        Spinner rm_spinner = (Spinner)control_panel_.findViewById(R.id.render_mode_spinner);
        rendermodeAdapter = new textSimpleListAdapter(activity, R.array.rendering_mode);
        rendermodeAdapter.setTitleById(0);
        rm_spinner.setAdapter(rendermodeAdapter);

        Spinner color_spinner = (Spinner)control_panel_.findViewById(R.id.render_color_spinner);
        colorAdapter = new textSimpleListAdapter(activity, R.array.color_schemes);
        colorAdapter.setTitleById(0);
        color_spinner.setAdapter(colorAdapter);

        Reset();
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
}

package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class renderUIs extends BasePanel{
    private final WeakReference<UIsManager> mUIManagerRef;
    private Spinner seekbar_spinner;
    private SeekbarAdapter seekbarAdapter = null;
    private textSimpleListAdapter rendermodeAdapter, colorAdapter;
    final private static int RAYCAST_ID = 1;
    private static String CHECK_TEXRAY_NAME;
    public renderUIs(final Activity activity, UIsManager manager, ViewGroup parent_view){
        super(activity, parent_view);
        mUIManagerRef = new WeakReference<>(manager);
        //panels
        final LayoutInflater mInflater = LayoutInflater.from(activity);
        View tune_panel_ = mInflater.inflate(R.layout.tune_panel, parent_view, false);
        View control_panel_ = mInflater.inflate(R.layout.transfunc_panel, parent_view, false);

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
        rm_spinner.setAdapter(rendermodeAdapter);

        Spinner color_spinner = (Spinner)control_panel_.findViewById(R.id.render_color_spinner);
        colorAdapter = new colorListAdapter(activity, R.array.color_schemes);
        color_spinner.setAdapter(colorAdapter);

        sub_panels_.add(tune_panel_);
        sub_panels_.add(control_panel_);

        setup_checks(R.array.render_check_params, R.array.render_check_values);
        CHECK_TEXRAY_NAME = activity.getResources().getString(R.string.texray_check_name);
    }
    public void Reset(){
        Resources res = actRef.get().getResources();
        seekbarAdapter.Reset();
        //render mode should be the first to set!!
        int rm_id = Integer.parseInt(res.getString(R.string.default_render_mode_id));
        rendermodeAdapter.setTitleById(rm_id);
        int color_id = Integer.parseInt(res.getString(R.string.default_color_mode_id));
        colorAdapter.setTitleById(color_id);
    }
    public boolean isRaycasting(){return ((renderListAdapter)rendermodeAdapter).getRenderingModeById() == RAYCAST_ID;}
    private void onTexRaySwitch(boolean isRaycast){
        seekbar_spinner.setAdapter(seekbarAdapter.getListAdapter(isRaycast?1:0));
        JUIInterface.JUIsetChecks(CHECK_TEXRAY_NAME, isRaycast);
    }
    public void showHidePanel(boolean show_panel){
        super.showHidePanel(show_panel);
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

        int current_id = -1;
        renderListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void onItemClick(int position){
//            if(position == current_id) return;
            mUIManagerRef.get().onTexRaySwitch(position == RAYCAST_ID);
            onTexRaySwitch(RAYCAST_ID == position);
            current_id = position;
        }
        void setTitleById(int id){
//            if(id == current_id) return;
            super.setTitleById(id);
            mUIManagerRef.get().onTexRaySwitch(RAYCAST_ID == id);
            onTexRaySwitch(RAYCAST_ID == id);
            current_id = id;
        }
        int getRenderingModeById(){return current_id;}
    }
    private class colorListAdapter extends textSimpleListAdapter{
        colorListAdapter(Context context, int arrayId){
            super(context, arrayId);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            JUIInterface.JuisetColorScheme(id);
        }
        void onItemClick(int position){
            JUIInterface.JuisetColorScheme(position);
        }
    }
}

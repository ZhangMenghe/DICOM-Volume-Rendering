package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.view.ViewGroup;

import com.google.common.primitives.Booleans;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.DUIs.BasePanel;
import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.DUIs.cutplaneUIs;
import helmsley.vr.DUIs.mainUIs;
import helmsley.vr.DUIs.maskUIs;
import helmsley.vr.DUIs.renderUIs;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";

    //Panels
    private mainUIs main_panel_;
    private LinkedHashMap<Integer, BasePanel> sub_panels_;

    final static public int tex_id=0, raycast_id=1;
    static private int current_texray_id = -1;
    private final static Integer[] sub_panel_name_ids_ ={
            R.string.panel_rendering_name,
            R.string.panel_cut_name,
            R.string.panel_mask_name
    };

    UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);

        final ViewGroup parent_view = (ViewGroup)activity_.findViewById(R.id.parentPanel);
        main_panel_ = new mainUIs(activity_, parent_view, this);
        sub_panels_ = new LinkedHashMap<>();

        //order matters
        sub_panels_.put(sub_panel_name_ids_[0], new renderUIs(activity_, this, parent_view));
        sub_panels_.put(sub_panel_name_ids_[1], new cutplaneUIs(activity_, parent_view));
        sub_panels_.put(sub_panel_name_ids_[2], new maskUIs(activity_, parent_view));
        RequestReset();
    }

    public void onTexRaySwitch(boolean isRaycast){
        current_texray_id = isRaycast?raycast_id:tex_id;
        sub_panels_.get(R.string.panel_cut_name).onTexRayChange(isRaycast);
    }
    public void onCuttingPlaneSwitch(boolean isPanelOn){
        sub_panels_.get(R.string.panel_cut_name).showHidePanel(isPanelOn, current_texray_id==raycast_id);
    }
    public void onRenderingSwitch(boolean isPanelOn){
        sub_panels_.get(R.string.panel_rendering_name).showHidePanel(isPanelOn);
    }
    public void onMaskPanelSwitch(boolean isPanelOn){
        sub_panels_.get(R.string.panel_mask_name).showHidePanel(isPanelOn);
    }
    public void RequestReset(){

        Resources res = actRef.get().getResources();
        main_panel_.Reset();
        boolean[] panel_show_status = main_panel_.getPanelStatus();
        //check params
        ArrayList<String> check_items_param = new ArrayList<>();
        ArrayList<Boolean> check_values_param = new ArrayList<>();

        for(int i=0;i<panel_show_status.length;i++){
            BasePanel p = sub_panels_.get(sub_panel_name_ids_[i]);
            p.Reset();
            p.showHidePanel(panel_show_status[i]);
            p.setCheckParams(res, check_items_param, check_values_param);
        }

        JUIInterface.JUIonReset(check_items_param.size(), check_items_param.toArray(new String[0]), Booleans.toArray(check_values_param));
    }
    void updateOnFrame(){
        main_panel_.updateOnFrame();
    }
}

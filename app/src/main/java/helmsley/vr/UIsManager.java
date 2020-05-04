package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.view.ViewGroup;

import com.google.common.primitives.Booleans;
import com.google.common.primitives.Floats;

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

    //not pretty, but the most convenient solution
    final static public int tex_id=0, raycast_id=1;
    static private int current_texray_id = -1;

    //Panels
    private mainUIs main_panel_;
    private LinkedHashMap<Integer, BasePanel> sub_panels_;
    private final static Integer[] sub_panel_name_ids_ ={
            R.string.panel_rendering_name,
            R.string.panel_cut_name,
            R.string.panel_mask_name
    };
    private final static float[]default_vol_pose={0,0,0,1,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    private final static float[]default_cam_pose={0,0,3.0f,0,1,0,0,0,2.0f};

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
    public void RequestResetWithTemplate(LinkedHashMap map){
        if(map.isEmpty()) return;
        Resources res = actRef.get().getResources();
        //prepare check params
        ArrayList<String> check_items_param = new ArrayList<>();
        ArrayList<Boolean> check_values_param = new ArrayList<>();
        for(BasePanel p: sub_panels_.values())
            p.ResetWithTemplate(map, check_items_param, check_values_param);
        //extract vol and cam pose
        float[] vol_pose = default_vol_pose.clone();float[] cam_pose=default_cam_pose.clone();
        LinkedHashMap volmap = (LinkedHashMap) map.getOrDefault("volume", null);
        if(volmap != null){
            float[] pos = Floats.toArray((ArrayList<Float>)volmap.getOrDefault("pos", new ArrayList<Float>()));
            if(pos.length == 3) System.arraycopy(pos, 0, vol_pose, 0, 3);
            float[] scale = Floats.toArray((ArrayList<Float>)volmap.getOrDefault("scale", new ArrayList<Float>()));
            if(scale.length == 3) System.arraycopy(scale, 0, vol_pose, 3, 3);
            float[] rotation = Floats.toArray((ArrayList<Float>)volmap.getOrDefault("rotation", new ArrayList<Float>()));
            if(rotation.length == 16) System.arraycopy(rotation, 0, vol_pose, 6, 16);
        }
        LinkedHashMap cammap = (LinkedHashMap) map.getOrDefault("camera", null);
        if(cammap != null){
            float[] pos = Floats.toArray((ArrayList<Float>)cammap.getOrDefault("pos", new ArrayList<Float>()));
            if(pos.length == 3) System.arraycopy(pos, 0, cam_pose, 0, 3);
            float[] up = Floats.toArray((ArrayList<Float>)cammap.getOrDefault("up", new ArrayList<Float>()));
            if(up.length == 3) System.arraycopy(up, 0, cam_pose, 3, 3);
            float[] center = Floats.toArray((ArrayList<Float>)cammap.getOrDefault("center", new ArrayList<Float>()));
            if(center.length == 3) System.arraycopy(center, 0, cam_pose, 6, 3);
        }

        JUIInterface.JUIonReset(check_items_param.size(),
                check_items_param.toArray(new String[0]),
                Booleans.toArray(check_values_param),
                vol_pose, cam_pose);
    }
    public void RequestReset(){
        Resources res = actRef.get().getResources();
        main_panel_.Reset();
        boolean[] panel_show_status = main_panel_.getPanelStatus();
        //check params
        ArrayList<String> check_items_param = new ArrayList<>();
        ArrayList<Boolean> check_values_param = new ArrayList<>();

        for(int i=0;i<sub_panels_.size();i++){
            BasePanel p = sub_panels_.get(sub_panel_name_ids_[i]);
            p.Reset();
            p.showHidePanel(panel_show_status[i]);
            p.setCheckParams(res, check_items_param, check_values_param);
        }
        JUIInterface.JUIonReset(check_items_param.size(),
                check_items_param.toArray(new String[0]),
                Booleans.toArray(check_values_param),
                default_vol_pose, default_cam_pose);
    }
    void updateOnFrame(){
        main_panel_.updateOnFrame();
    }
}

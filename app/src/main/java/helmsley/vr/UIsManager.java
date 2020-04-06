package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.Spinner;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.DUIs.BasePanel;
import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.DUIs.checkpanelAdapter;
import helmsley.vr.DUIs.cutplaneUIs;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.DUIs.funcAdapter;
import helmsley.vr.DUIs.maskUIs;
import helmsley.vr.DUIs.renderUIs;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";
    //Panels
    private LinkedHashMap<Integer, BasePanel> sub_panels_;
//    private BasePanel cuttingController;
//    private BasePanel renderController;
//    private BasePanel masksController;

    // UIs
    private Spinner spinner_check;
    private dialogUIs dialogController;
    //Spinner adapter
    private checkpanelAdapter cb_panel_adapter = null;

    final static public int tex_id=0, raycast_id=1;
    final static private int PANEL_NUM = 3;
    static private int current_texray_id = -1;
    private final static Integer[] sub_panel_name_ids_ ={
            R.string.panel_rendering_name,
            R.string.panel_cut_name,
            R.string.panel_mask_name
    };

    UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        dialogController = new dialogUIs(activity_);
        setupTopPanelSpinners();
        setupSubPanels(activity_);
        RequestReset();
    }
    private void setupSubPanels(Activity activity_){
        final ViewGroup parent_view = (ViewGroup)activity_.findViewById(R.id.parentPanel);
        sub_panels_ = new LinkedHashMap<>();

        //order matters
        sub_panels_.put(sub_panel_name_ids_[0], new renderUIs(activity_, this, parent_view));
        sub_panels_.put(sub_panel_name_ids_[1], new cutplaneUIs(activity_, parent_view));
        sub_panels_.put(sub_panel_name_ids_[2], new maskUIs(activity_, parent_view));
    }
    private void setupTopPanelSpinners(){
        //checkbox spinners
        spinner_check =  (Spinner)actRef.get().findViewById(R.id.checkPanelSpinner);
        cb_panel_adapter = new checkpanelAdapter(actRef.get(), this);
        spinner_check.setAdapter(cb_panel_adapter);

        //function spinners
        Spinner spinner_func = (Spinner) actRef.get().findViewById(R.id.funcSpinner);
        funcAdapter fAdapter = new funcAdapter(actRef.get(), this);
        spinner_func.setAdapter(fAdapter.createFuncAdapter());
    }
    public void RequestSetupServerConnection(){
        dialogController.SetupConnectRemote();
    }
    public void RequestSetupLocalDataServer(){
        dialogController.SetupConnectLocal();
    }
    public static int getTexRayIdx(){
        return current_texray_id;
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
        for(BasePanel p: sub_panels_.values()){
            p.Reset();
            p.showHidePanel(false);
        }

        cb_panel_adapter.Reset();
        spinner_check.setAdapter(cb_panel_adapter);

        //reset values
        Resources res = actRef.get().getResources();
//        TypedArray params = res.obtainTypedArray(R.array.checkJNIParams);
//        if(params.length()!=PANEL_NUM){
//            Log.e(TAG, "RequestReset: number of panels NOT equal to check parameters" );
//            return;
//        }
//        //order matters!
//        ArrayList<Boolean> check_values = new ArrayList<>();
//        for(int i=0; i<PANEL_NUM; i++){
//            check_values = renderController.resetCheckParams(0, check_values);
//
//        }
        String[] check_items = res.getStringArray(R.array.checkParams);
        boolean[] values = new boolean[check_items.length];
        for(int i=0;i<values.length;i++)values[i] = false;
//        values[0] = false;//renderController.isRaycasting();
//        values[1] = false;//masksController.isMaskOn();
//        values = cuttingController.setCuttingStatus(2, values);
        JUIInterface.JUIonReset(check_items.length, check_items, values);
    }
    void updateOnFrame(){
        dialogController.updateOnFrame();
    }
}

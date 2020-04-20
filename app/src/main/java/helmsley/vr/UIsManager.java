package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.Spinner;

import com.google.common.primitives.Booleans;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

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

        setupTopPanelSpinners();
        setupSubPanels(activity_);
        RequestReset();
    }
    private void setupSubPanels(Activity activity_){
        final ViewGroup parent_view = (ViewGroup)activity_.findViewById(R.id.parentPanel);
        dialogController = new dialogUIs(activity_, parent_view);

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
        Resources res = actRef.get().getResources();
        String[] check_items_panel = new String[sub_panel_name_ids_.length];
        boolean[] check_values_panel = new boolean[sub_panel_name_ids_.length];
        //check params
        ArrayList<String> check_items_param = new ArrayList<>();
        ArrayList<Boolean> check_values_param = new ArrayList<>();

        TypedArray check_values_type = res.obtainTypedArray(R.array.checkShowPanelValues);

        for(int i=0;i<check_items_panel.length;i++){
            check_items_panel[i] = res.getString(sub_panel_name_ids_[i]);
            check_values_panel[i] = check_values_type.getBoolean(i, false);

            BasePanel p = sub_panels_.get(sub_panel_name_ids_[i]);
            p.Reset();
            p.showHidePanel(check_values_panel[i]);
            p.setCheckParams(res,check_items_param, check_values_param);
        }
        check_values_type.recycle();

        cb_panel_adapter.Reset(check_items_panel, check_values_panel);
        spinner_check.setAdapter(cb_panel_adapter);

        Log.e(TAG, "=======RequestReset: " + check_items_param.size());
        JUIInterface.JUIonReset(check_items_param.size(), check_items_param.toArray(new String[0]), Booleans.toArray(check_values_param));
    }
    void updateOnFrame(){
        dialogController.updateOnFrame();
    }
}

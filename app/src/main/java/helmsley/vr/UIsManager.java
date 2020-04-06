package helmsley.vr;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.Spinner;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

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
    private cutplaneUIs cuttingController;
    private renderUIs renderController;
    private maskUIs masksController;

    // UIs
    private Spinner spinner_check;
    private dialogUIs dialogController;
    //Spinner adapter
    private checkpanelAdapter cb_panel_adapter = null;

    final static public int tex_id=0, raycast_id=1;
    final static private int PANEL_NUM = 3;
    static private int current_texray_id = -1;

    UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        dialogController = new dialogUIs(activity_);
        setupSubPanels(activity_);
        setupTopPanelSpinners();
        RequestReset();
    }
    private void setupSubPanels(Activity activity_){
        final ViewGroup parent_view = (ViewGroup)activity_.findViewById(R.id.parentPanel);
        cuttingController = new cutplaneUIs(activity_, parent_view);
        renderController = new renderUIs(activity_, this, parent_view);
        masksController = new maskUIs(activity_, parent_view);
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
        cuttingController.onCuttingStateChange(isRaycast);
    }
    public void onCuttingPlaneSwitch(boolean isPanelOn){
        cuttingController.onCuttingStateChange(isPanelOn, current_texray_id==raycast_id);
    }
    public void onRenderingSwitch(boolean isPanelOn){
        renderController.showHidePanel(isPanelOn);
    }
    public void onMaskPanelSwitch(boolean isPanelOn){
        masksController.showHidePanel(isPanelOn);
    }
    public void RequestReset(){
        renderController.showHidePanel(false);
        renderController.Reset(actRef.get().getResources());

        cuttingController.showHidePanel(false);
        cuttingController.Reset();

        masksController.showHidePanel(false);
        masksController.Reset();

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
        values[0] = renderController.isRaycasting();
        values[1] = masksController.isMaskOn();
        values = cuttingController.setCuttingStatus(2, values);
        JUIInterface.JUIonReset(check_items.length, check_items, values);
    }
    void updateOnFrame(){
        dialogController.updateOnFrame();
    }
}

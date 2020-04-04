package helmsley.vr;

import android.app.Activity;
import android.view.ViewGroup;
import android.widget.Spinner;
import java.lang.ref.WeakReference;
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

    // UIs
    private Spinner spinner_tune;
    private Spinner spinner_check;

    private dialogUIs dialogController;
    private maskUIs masksController;

    //Spinner adapter
    private checkpanelAdapter cb_panel_adapter = null;

    final static public int tex_id=0, raycast_id=1;
    static private int current_texray_id = -1;

    UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        dialogController = new dialogUIs(activity_);
        setupSubPanels(activity_);
        setupTopPanelSpinners();

    }
    private void setupSubPanels(Activity activity_){
        final ViewGroup parent_view = (ViewGroup)activity_.findViewById(R.id.parentPanel);
        cuttingController = new cutplaneUIs(activity_, parent_view);
        renderController = new renderUIs(activity_, parent_view);
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
        renderController.onTexRaySwitch(isRaycast);
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
        renderController.Reset();
        cuttingController.Reset();
        masksController.Reset();

        cb_panel_adapter.Reset();
        spinner_check.setAdapter(cb_panel_adapter);
        JUIInterface.JUIonReset();
    }
    void updateOnFrame(){
        dialogController.updateOnFrame();
    }
}

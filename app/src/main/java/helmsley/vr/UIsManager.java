package helmsley.vr;

import android.app.Activity;
import android.widget.Spinner;

import java.lang.ref.WeakReference;
import helmsley.vr.DUIs.SeekbarAdapter;
import helmsley.vr.DUIs.checkboxAdapter;
import helmsley.vr.DUIs.cutplaneUIs;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.DUIs.funcAdapter;

public class UIsManager {
    private final WeakReference<Activity> actRef;
    final static String TAG = "UIsManager";
    // UIs
    private Spinner spinner_tune, spinner_check, spinner_func;
    private cutplaneUIs cuttingController;
    protected dialogUIs dialogController;

    //Spinner adapter
    SeekbarAdapter seekbarAdapter;
    checkboxAdapter cbAdapter;

    final static public int tex_id=0, raycast_id=1;
    static private int current_texray_id = -1;
    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        dialogController = new dialogUIs(activity_);
        cuttingController = new cutplaneUIs(activity_);
        setupTopPanelSpinners();
    }

    private void setupTopPanelSpinners(){
        //Tune spinners
        spinner_tune =  (Spinner)actRef.get().findViewById(R.id.tuneSpinner);
        seekbarAdapter = new SeekbarAdapter(actRef.get(), this);

        //checkbox spinners
        spinner_check =  (Spinner)actRef.get().findViewById(R.id.checkSpinner);
        cbAdapter = new checkboxAdapter(actRef.get(), this);
        spinner_check.setAdapter(cbAdapter.getListAdapter());

        //function spinners
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);
        funcAdapter fAdapter = new funcAdapter(actRef.get(), this);
        spinner_func.setAdapter(fAdapter.createFuncAdapter());
    }
    public void RequestSetupServerConnection(){
        dialogController.SetupConnect();
    }
    public static int getTexRayIdx(){
        return current_texray_id;
    }
    public void onTexRaySwitch(boolean isRaycast){
        spinner_tune.setAdapter(seekbarAdapter.getListAdapter(isRaycast?raycast_id:tex_id));
        current_texray_id = isRaycast?raycast_id:tex_id;
        cuttingController.onCuttingStateChange(isRaycast);
    }
    public void onCuttingPlaneSwitch(boolean isCutting){
        cuttingController.onCuttingStateChange(isCutting, current_texray_id==raycast_id);
    }
    public void RequestReset(){
        cuttingController.Reset();
        seekbarAdapter.Reset();
        cbAdapter.Reset();
        spinner_check.setAdapter(cbAdapter.getListAdapter());
    }

}

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

    final private int tex_id=0, raycast_id=1;
    public UIsManager(final Activity activity_){
        actRef = new WeakReference<>(activity_);
        dialogController = new dialogUIs(activity_);
        cuttingController = new cutplaneUIs(activity_);
        InitUIs();
    }


    private void InitUIs(){
        setupTopPanelSpinners();

    }

    private void setupTopPanelSpinners(){
        //Tune spinners
        spinner_tune =  (Spinner)actRef.get().findViewById(R.id.tuneSpinner);
        SeekbarAdapter seekbarAdapter = new SeekbarAdapter(actRef.get());
        spinner_tune.setAdapter(seekbarAdapter.createListAdapter(raycast_id));

        //checkbox spinners
        spinner_check =  (Spinner)actRef.get().findViewById(R.id.checkSpinner);
        checkboxAdapter cbAdapter = new checkboxAdapter(actRef.get());
        spinner_check.setAdapter(cbAdapter.createListAdapter(tex_id));

        //function spinners
        spinner_func = (Spinner)actRef.get().findViewById(R.id.funcSpinner);
        funcAdapter fAdapter = new funcAdapter(actRef.get(), this);
        spinner_func.setAdapter(fAdapter.createFuncAdapter());
    }
    public void RequestSetupServerConnection(){
        dialogController.SetupConnect();
    }

}

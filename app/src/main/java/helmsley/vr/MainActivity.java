package helmsley.vr;

import android.os.Bundle;
import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.proto.fileTransferClient;

public class MainActivity extends GLActivity {
    final static String TAG = "Main_Activity";
    static {
        System.loadLibrary("vrAndroid");
    }
    //ui
    protected UIsManager ui_manager;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ui_manager = new UIsManager(this);
    }
//    protected boolean setupResource(){
//        //do nothing in this case
//        return true;
//    }

    @Override
    protected void updateOnFrame(){
        if(fileTransferClient.finished) {JNIInterface.JNIAssembleVolume();dialogUIs.FinishProgress(); fileTransferClient.finished=false;}
//        if(fileTransferClient.finished_mask){JNIInterface.JNIAssembleMask();fileTransferClient.finished_mask = false;}
        super.updateOnFrame();
    }
}

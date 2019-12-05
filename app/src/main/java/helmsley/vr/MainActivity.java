package helmsley.vr;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;

import java.util.List;

import helmsley.vr.DUIs.dialogUIs;
import helmsley.vr.Utils.dcmImage;
import helmsley.vr.Utils.fileUtils;

import static helmsley.vr.Utils.fileUtils.loadImagesFromDir;
import helmsley.vr.proto.fileTransferClient;

public class MainActivity extends GLActivity {
    final static String TAG = "Main_Activity";
    static {
        System.loadLibrary("vrNative");
    }
    //ui
    protected UIsManager ui_manager;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ui_manager = new UIsManager(this);
    }
    protected boolean setupResource(){
        //do nothing in this case
        return true;
    }

    @Override
    protected void updateOnFrame(){
//        if(fileTransferClient.finished) {JNIInterface.JNIAssembleVolume();dialogUIs.FinishProgress(); fileTransferClient.finished=false;}
//        if(fileTransferClient.finished_mask){JNIInterface.JNIAssembleMask();fileTransferClient.finished_mask = false;}
//        super.updateOnFrame();
//        uiController.updateFPS();
    }
}

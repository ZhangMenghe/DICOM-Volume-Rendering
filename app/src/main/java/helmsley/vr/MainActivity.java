package helmsley.vr;

import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import helmsley.vr.Utils.dcmImage;
import helmsley.vr.Utils.fileUtils;

public class MainActivity extends GLActivity {
    final static String TAG = "Main_Activity";
    static {
        System.loadLibrary("vrNative");
    }

    //dcm
    List<String> file_path_lst;
    int file_nums;
    ArrayList<dcmImage> dcm_images = new ArrayList<>();
    //ui
    protected UIsController uiController;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        uiController = new UIsController(this);
    }
    protected boolean setupResource(){
        super.setupResource();
        setupDCMI();
        return true;
    }
    private void setupDCMI() {
        file_path_lst = fileUtils.getListFilesFromDir(new File(ass_copy_dst+"/"+getString(R.string.config_volname)));
        file_nums = file_path_lst.size();
        Log.e(TAG, ass_copy_dst+": =====num: " + file_nums );
        for(String file_path : file_path_lst)
            dcm_images.add(new dcmImage(file_path));

        //pass down to native
        JNIInterface.JNIsendDCMImgs(dcm_images.toArray(new dcmImage[0]), file_nums);
    }
    @Override
    protected void updateOnFrame(){
        super.updateOnFrame();
        uiController.updateFPS();
    }
}

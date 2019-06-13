package helmsley.vr;

import android.os.Bundle;

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
//        uiController = new UIsController(this);
        ass_copy_src = "dicom-images";
    }
    protected void setupResource(){
        super.setupResource();
        setupDCMI();
    }
    private void setupDCMI() {
        file_path_lst = fileUtils.getListFilesFromDir(new File(ass_copy_dst +"/sample/"));
        file_nums = file_path_lst.size();

        for(String file_path : file_path_lst)
            dcm_images.add(new dcmImage(file_path));

        //pass down to native
        JNIInterface.JNIsendDCMImgs(dcm_images.toArray(new dcmImage[0]), file_nums);
    }
}

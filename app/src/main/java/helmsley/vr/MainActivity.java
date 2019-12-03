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

    //dcm
    List<String> file_path_lst, msk_path_lst;
    int file_nums;
    ArrayList<dcmImage> dcm_images = new ArrayList<>();
    //ui
    protected UIsController uiController;
    //downloader
    fileTransferClient downloader;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        uiController = new UIsController(this);
//        downloader = new fileTransferClient("137.110.112.178","23333");
    }
    protected boolean setupResource(){
        //do nothing in this case
//        super.setupResource();
//        setupDCMI();
//        downloader.Run();
        return true;
    }
    private void setupDCMI() {
        file_path_lst = fileUtils.getListFilesFromDir(new File(ass_copy_dst+"/"+getString(R.string.config_volname)));
        ArrayList<Bitmap> masks = loadImagesFromDir(ass_copy_dst+"/"+getString(R.string.config_volname)+getString(R.string.config_mask_tail), true);

        file_nums = file_path_lst.size();
        Log.e(TAG, ass_copy_dst+": =====num: " + file_nums );
        if(getString(R.string.config_volname).equals("sample")){
            for(int i=0; i<file_nums; i++)
                dcm_images.add(new dcmImage(file_path_lst.get(i), -1.0f));
        }else{
            for(int i=0; i<file_nums; i++)
                dcm_images.add(new dcmImage(file_path_lst.get(i)));
        }

        JNIInterface.JNIsendDCMImgs(dcm_images.toArray(new dcmImage[0]), masks.toArray(), file_nums);

    }
    @Override
    protected void updateOnFrame(){
        if(fileTransferClient.finished) {JNIInterface.JNIAssembleVolume();dialogUIs.FinishProgress(); fileTransferClient.finished=false;}
        super.updateOnFrame();
        uiController.updateFPS();
    }
}

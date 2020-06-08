package helmsley.vr;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Toast;

public class MainActivity extends GLActivity {
    final static String TAG = "Main_Activity";
    static {
        System.loadLibrary("vrAndroid");
    }
    //ui
    protected UIsManager ui_manager;
    private dicomManager dcm_manager;
    public static boolean permission_granted = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("imebra_lib");
        ui_manager = new UIsManager(this);
        dcm_manager = new dicomManager(this);
    }

    @Override
    protected void updateOnFrame(){
        ui_manager.updateOnFrame();
        dcm_manager.updateOnFrame();
        super.updateOnFrame();
    }

    /*
    Here we get the response from the file selector. We use the returned URI to open an
    InputStream which we push to the DICOM codec through a PIPE.

    It would be simpler to just use a file name with the DICOM codec, but this is difficult
    to obtain from the file selector dialog and would not allow to load also files from external
    sources (e.g. the Google Drive).
 */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(requestCode == 123 && resultCode == RESULT_OK)
            dcm_manager.Run(data.getData());
    }
    @Override
    protected void checkPermissions(){
        String[] PERMISSIONS = {
                android.Manifest.permission.READ_EXTERNAL_STORAGE,
        };
        for(String rpermission:PERMISSIONS){
            if(ActivityCompat.checkSelfPermission(this, rpermission) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS, 123);
                return;
            }
        }
        permission_granted = true;
    }
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults ){
        if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            permission_granted = true;
        } else {
            Toast.makeText(
                    this,
                    "Permission denied to read your External storage, try to enable it via system setting",
                    Toast.LENGTH_SHORT
            ).show();
        }
    }
}

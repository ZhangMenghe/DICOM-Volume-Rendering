package helmsley.vr;

import android.os.Bundle;

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

    @Override
    protected void updateOnFrame(){
        ui_manager.updateOnFrame();
        super.updateOnFrame();
    }
}

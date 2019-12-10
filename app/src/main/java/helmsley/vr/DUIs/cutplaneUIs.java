package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.design.widget.FloatingActionButton;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;

import java.lang.ref.WeakReference;

import helmsley.vr.R;

import static helmsley.vr.UIsController.JUIsetParam;

public class cutplaneUIs {
    private final WeakReference<Activity> actRef;
    final static String TAG = "cutplaneUIs";

    //widgets
    private SeekBar seek_bar_;
    private FloatingActionButton button_;

    //panel
    private View panel_tune_;//bottom

    public cutplaneUIs(final Activity activity){
        actRef = new WeakReference<>(activity);

        panel_tune_ = (View)activity.findViewById(R.id.bottomPanel);
        panel_tune_.setVisibility(View.INVISIBLE);

        seek_bar_ = (SeekBar)activity.findViewById(R.id.cuttingSeekBar);
        seek_bar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                Log.e(TAG, "====onProgressChanged: " );
//                if(!b) return;
//                JUIsetParam("cutting", 1.0f * i / seek_value_pair.second);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        button_ = activity.findViewById(R.id.cutting_button);
        button_.setOnTouchListener(new View.OnTouchListener(){
            float down_v_x, down_e_x;
            float e_v_offset;
            public boolean onTouch(View view, MotionEvent event) {
                switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN:
                        down_v_x = view.getX();
                        down_e_x = event.getRawX();
                        e_v_offset = down_v_x - down_e_x;
                        break;

                    case MotionEvent.ACTION_MOVE:
                        JUIsetParam("cutting", event.getRawX() - down_e_x);
                        view.setX(e_v_offset + event.getRawX());
                        break;

                    case MotionEvent.ACTION_UP:
                        view.setX(down_v_x);
                        break;
                    case MotionEvent.ACTION_BUTTON_PRESS:

                    default:
                        return false;
                }
                return true;
            }
        });
    }
}

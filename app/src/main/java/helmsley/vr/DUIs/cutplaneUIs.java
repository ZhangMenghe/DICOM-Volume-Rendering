package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.res.TypedArray;
import android.support.design.widget.FloatingActionButton;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;
import android.widget.Toast;

import java.lang.ref.WeakReference;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class cutplaneUIs {
    private final WeakReference<Activity> actRef;
    final static String TAG = "cutplaneUIs";

    //widgets
    private SeekBar seek_bar_;
    private FloatingActionButton button_;

    //panel
    private View panel_tune_;//bottom

    private boolean isCutting = false;
    public cutplaneUIs(final Activity activity){
        actRef = new WeakReference<>(activity);

        panel_tune_ = (View)activity.findViewById(R.id.bottomPanel);
        panel_tune_.setVisibility(View.INVISIBLE);

        seek_bar_ = (SeekBar)activity.findViewById(R.id.cuttingSeekBar);
        String params[] = activity.getResources().getStringArray(R.array.cutting_plane);

        int max_seek_value = Integer.valueOf(params[1]);
        seek_bar_.setProgress((int)(Float.valueOf(params[0]) * max_seek_value));
        seek_bar_.setMax(max_seek_value);
        seek_bar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                JUIInterface.JUIsetCuttingPlane(UIsManager.tex_id, 1.0f * i / max_seek_value);
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
                        JUIInterface.JUIsetCuttingPlane(UIsManager.raycast_id, event.getRawX() - down_e_x);
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

    public void onCuttingStateChange(boolean isCutting_, boolean isRaycast){
        isCutting = isCutting_;
        if(isCutting){
            panel_tune_.setVisibility(View.VISIBLE);
            if(isRaycast){
                button_.show();
                seek_bar_.setVisibility(View.INVISIBLE);
                Toast.makeText(actRef.get(), "Drag Bottom Button to Change", Toast.LENGTH_LONG).show();
            }else{
                button_.hide();
                seek_bar_.setVisibility(View.VISIBLE);
                Toast.makeText(actRef.get(), "Use Bottom SeekBar to Cut", Toast.LENGTH_LONG).show();
            }
        }else{
            panel_tune_.setVisibility(View.INVISIBLE);
        }
    }
    public void onCuttingStateChange(boolean isRaycast){
        onCuttingStateChange(isCutting, isRaycast);
    }
}

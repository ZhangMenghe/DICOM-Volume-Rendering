package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.constraint.ConstraintLayout;
import android.support.design.widget.FloatingActionButton;
import android.util.Pair;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Toast;

import helmsley.vr.R;

import static helmsley.vr.UIsController.JUIsetParam;
import static helmsley.vr.UIsController.JUIsetSwitches;

public class cuttingUIs {
    public static Activity activity;
    final static String TAG = "cuttingUIs";

    private View panel_tune_, panel_set_, panel_set_sub_;
    private SeekBar seek_bar_;
    private FloatingActionButton button_;

    private boolean cut_panel_on = false;
    private static Pair<Float, Integer> seek_value_pair = new Pair<>(1.0f, 50);

    final private static int finger_manipulate_id = R.id.radio_vol, cut_from_id = R.id.radio_fromview;
    
    public cuttingUIs(final Activity activity_){
        activity = activity_;
        InitPanels();
        InitRadioGroup();
        InitButton();
    }
    public void onCuttingStateChange(boolean is_ray_cast, boolean change_to_show_cut, View raycast_conflict_panel){
        if(change_to_show_cut){
            panel_tune_.setVisibility(View.VISIBLE);
            if(is_ray_cast){
                panel_set_.setVisibility(View.VISIBLE);
                button_.setVisibility(View.VISIBLE);
                seek_bar_.setVisibility(View.INVISIBLE);

                raycast_conflict_panel.setVisibility(View.INVISIBLE);
                cut_panel_on = true;
                Toast.makeText(activity, "Drag Bottom Button to Change", Toast.LENGTH_LONG).show();
            }else{
                button_.setVisibility(View.INVISIBLE);
                seek_bar_.setVisibility(View.VISIBLE);
                Toast.makeText(activity, "Use Bottom SeekBar to Cut", Toast.LENGTH_LONG).show();
            }

        }else{
            panel_tune_.setVisibility(View.INVISIBLE);
            if(is_ray_cast){
                panel_set_.setVisibility(View.INVISIBLE);
                raycast_conflict_panel.setVisibility(View.VISIBLE);
                cut_panel_on = false;
            }
        }
    }
    public void onSeekBarChange(boolean isRaycast, Float value){
        if(isRaycast) return;
        seek_bar_.setMax(seek_value_pair.second);

        seek_bar_.setProgress((int)value.floatValue());
    }
    private void InitRadioGroup(){
        //radio group
        RadioGroup radioGroup_cut = (RadioGroup)activity.findViewById(R.id.cut_radioGroup);
        radioGroup_cut.check(finger_manipulate_id);
        JUIsetParam("mtarget", -1.0f);
        radioGroup_cut.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener(){
            public void onCheckedChanged(RadioGroup group, int checkedId){
                // This will get the radiobutton that has changed in its check state
                //                RadioButton checkedRadioButton = (RadioButton)group.findViewById(checkedId);
                panel_set_sub_.setVisibility(View.INVISIBLE);
                switch (checkedId){
                    case R.id.radio_vol:
                        JUIsetParam("mtarget", -1.0f);
                        break;
                    case R.id.radio_plane:
                        if(cut_panel_on) panel_set_sub_.setVisibility(View.VISIBLE);
                        JUIsetParam("mtarget", 1.0f);
                        break;
                    case R.id.radio_sphere:
                        JUIsetParam("mtarget", 2.0f);
                        break;
                    default:
                        break;
                }
            }
        });
        RadioGroup radioGroup_cut_sub = (RadioGroup)activity.findViewById(R.id.cut_sub_radiogroup);
        radioGroup_cut_sub.check(cut_from_id);
        JUIsetSwitches("pfview", (cut_from_id == R.id.radio_fromview));
        radioGroup_cut_sub.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener(){
            public void onCheckedChanged(RadioGroup group, int checkedId){
                JUIsetSwitches("pfview", (checkedId == R.id.radio_fromview));
            }
        });
    }
    private void InitPanels(){
        panel_tune_ = (View)activity.findViewById(R.id.bottomPanel);
        panel_tune_.setVisibility(View.INVISIBLE);
        seek_bar_ = (SeekBar)activity.findViewById(R.id.cuttingSeekBar);
        seek_bar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                JUIsetParam("cutting", 1.0f * i / seek_value_pair.second);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        panel_set_ = (View)activity.findViewById(R.id.cutPanel);
        panel_set_.setVisibility(View.INVISIBLE);
        panel_set_sub_ = (View)activity.findViewById(R.id.cut_sub_panel);
        panel_set_sub_.setVisibility(View.INVISIBLE);
    }
    private void InitButton(){
        button_ = activity.findViewById(R.id.cutting_button);
        button_.setOnTouchListener(new View.OnTouchListener(){
            float down_v_x, down_e_x;
            public boolean onTouch(View view, MotionEvent event) {
                switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN:
                        down_v_x = view.getX();
                        down_e_x = event.getX();
                        break;

                    case MotionEvent.ACTION_MOVE:
                        view.setX(down_v_x + event.getX() - down_e_x);

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

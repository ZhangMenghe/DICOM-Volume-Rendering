package helmsley.vr.DUIs;

import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;

public class DUIHelpers {
    public static TranslateAnimation panelHiddenAction, panelShownAction;

    static {
        panelHiddenAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF,
                0.0f, Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                1.0f);
        panelHiddenAction.setDuration(500);

        panelShownAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                1.0f, Animation.RELATIVE_TO_SELF, .0f);
        panelShownAction.setDuration(500);
    }

    public static void ToggleShowView_animate(View panel, boolean isShowUp){
        if(isShowUp){
            panel.startAnimation(panelShownAction);
            panel.setVisibility(View.VISIBLE);

        }else{
            panel.startAnimation(panelHiddenAction);
            panel.setVisibility(View.GONE);
        }
    }
}

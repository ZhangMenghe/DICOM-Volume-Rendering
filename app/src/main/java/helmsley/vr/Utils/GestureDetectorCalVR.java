package  helmsley.vr.Utils;

import android.content.Context;
import android.graphics.PointF;
import android.util.Log;
import android.view.MotionEvent;

import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.UIsController;

import static java.lang.Math.abs;

public class GestureDetectorCalVR {
    private MultiFingerTapDetector multiDetector;
    final static String TAG = "CalVR_Gesture";
    public GestureDetectorCalVR(Context ctx){
        multiDetector = new MultiFingerTapDetector() {
            // Methods that need to be overridden
            //public abstract void testing();
            public void onOneFingerDown(MotionEvent event){
//                Log.e(TAG, "onOneFingerDown: ======down" );
//                UIsController.JUIonSingleTouchDown(event.getX(), event.getY());
                JUIInterface.JUIonSingleTouchDown(event.getX(), event.getY());
            }
            public void onOneFingerMove(MotionEvent event){
//                Log.e(TAG, "onOneFingerDown: ======move" );
//                UIsController.JUIonTouchMove(event.getX(), event.getY());
                JUIInterface.JUIonTouchMove(event.getX(), event.getY());

            }

            public void onFling(int pointerNum, float srcx, float srcy, float dstx, float dsty){}

            // LEFT(0) OR Right(1) Single Tap [UP]
            public  void onSingleTapUp(int pointerNum, MotionEvent event){
//                JniInterface.JNIonSingleTouchUp(pointerNum - 1, event.getX(), event.getY());
            }

            // TWO FINGERS (right mouse click)
            // RIGHT SINGLE TOUCH
            public void onMoreFingersDown(int pointerNum, MotionEvent event){
//                JniInterface.JNIonSingleTouchDown(pointerNum - 1, event.getX(), event.getY());
                if(pointerNum != 2) return;
                int id1 = event.findPointerIndex(event.getPointerId(0));
                int id2 = event.findPointerIndex(event.getPointerId(1));
                down_f1.set(event.getX(id1), event.getY(id1));
                down_f2.set(event.getX(id2), event.getY(id2));

                down_span.set(abs(down_f2.x - down_f1.x), abs(down_f2.y-down_f1.y));
                last_span = down_span;
                last_pos_pan.set(event.getX(), event.getY());
                down_dist = PointF.length(down_span.x, down_span.y);
            }
            public void onTwoFingersMove(MotionEvent event){
//                if(!hasSkipped && event.getEventTime() - event.getDownTime() <TIMEOUT) return;
//                hasSkipped = true;
                PointF cf1, cf2, gap;
                int id1 = event.findPointerIndex(event.getPointerId(0));
                int id2 = event.findPointerIndex(event.getPointerId(1));
                cf1 = new PointF(event.getX(id1), event.getY(id1));
                cf2 = new PointF(event.getX(id2), event.getY(id2));

                gap = new PointF(cf1.x - cf2.x, cf1.y - cf2.y );

                float curr_dist = PointF.length(gap.x, gap.y);
                float last_span_dist = PointF.length(last_span.x, last_span.y);
                float dist_ratio;

                if(curr_dist > down_dist){//could be up scaling or move
                    dist_ratio = curr_dist / down_dist;
                }else{
                    dist_ratio = down_dist / curr_dist;
                }

                if(dist_ratio<SCALE_DIST_THRESHOLD
                        && (cf1.x- down_f1.x) *(cf2.x - down_f2.x) + (cf1.y - down_f1.y) * (cf2.y - down_f2.y) > .0f ){
                    float cx = event.getX(), cy = event.getY();///UIsController.screen_height;
                    JUIInterface.JUIonPan(cx - last_pos_pan.x, cy-last_pos_pan.y);

//                    UIsController.JUIonPan(cx - last_pos_pan.x, cy-last_pos_pan.y);
                    last_pos_pan.set( cx, cy );
                }else{
//                    Log.e(TAG, "===onScale:  " + dist_ratio);
//                    UIsController.JUIonScale(curr_dist/last_span_dist, curr_dist/last_span_dist);
                    JUIInterface.JUIonScale(curr_dist/last_span_dist, curr_dist/last_span_dist);

                }
                last_span.set(gap);
            }
            public void onThreeFingersMove(MotionEvent event){}

            // ONE FINGER (left mouse click)
            // left double tap
            public void onOneFingerDoubleTap(float ex, float ey){
//                JNIInterface.JNIonDoubleTouch(0, ex, ey);
            }
            public void onOneFingerTripleTap(){}
            public void onOneFingerLongPress(){}

            // TWO FINGER DOUBLE: right double
            public void onTwoFingerDoubleTap(float ex, float ey){
//                JNIInterface.JNIonDoubleTouch(1, ex, ey);
            }

            public void onTwoFingerTripleTap(){}
            public void onTwoFingerLongPress(MotionEvent event){
//                Log.e(TAG, "=====onTwoFingerLongPress: ");
            }

            // THREE FINGER TAPS
            public void onThreeFingerDoubleTap(){}
            public void onThreeFingerTripleTap(){}
            public void onThreeFingerLongPress(MotionEvent event){}
        };

    }
    public boolean onTouchEvent(MotionEvent event) {
        multiDetector.onTouchEvent(event);
        return true;
    }
}


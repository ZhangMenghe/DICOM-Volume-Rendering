package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.annotation.NonNull;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.util.Arrays;

import helmsley.vr.R;

public class maskUIs {
    private final WeakReference<Activity> actRef;
    private View panel;
//    RecyclerView recyclerView;
    maskRecyclerViewAdapter RecyclerViewHorizontalAdapter;

    public maskUIs(final Activity activity) {
        actRef = new WeakReference<>(activity);
        panel = (View)activity.findViewById(R.id.maskPanel);

        RecyclerView recyclerView = (RecyclerView)activity.findViewById(R.id.mask_recycle);
        recyclerView.setHasFixedSize(true);
        LinearLayoutManager HorizontalLayout = new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false);
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        recyclerView.setLayoutManager(layout_manager);
        recyclerView.setLayoutManager(HorizontalLayout);

        //adapter
        String mask_list[] = activity.getResources().getStringArray(R.array.masks_list);
        recyclerView.setAdapter(new maskRecyclerViewAdapter(Arrays.asList(mask_list)));

        recyclerView.addOnItemTouchListener(new RecyclerView.OnItemTouchListener() {
            GestureDetector gestureDetector = new GestureDetector(activity, new GestureDetector.SimpleOnGestureListener() {

                @Override public boolean onSingleTapUp(MotionEvent motionEvent) {

                    return true;
                }

            });

            @Override
            public boolean onInterceptTouchEvent(@NonNull RecyclerView recyclerView, @NonNull MotionEvent motionEvent) {
                View ChildView = recyclerView.findChildViewUnder(motionEvent.getX(), motionEvent.getY());

                if(ChildView != null && gestureDetector.onTouchEvent(motionEvent)) {

                    //Getting clicked value.
                    int RecyclerViewItemPosition = recyclerView.getChildAdapterPosition(ChildView);

                    // Showing clicked item value on screen using toast message.
//                    Toast.makeText(activity, RecyclerViewItemPosition, Toast.LENGTH_LONG).show();
//Number.get(RecyclerViewItemPosition)
                }

                return false;
            }

            @Override
            public void onTouchEvent(@NonNull RecyclerView recyclerView, @NonNull MotionEvent motionEvent) {

            }

            @Override
            public void onRequestDisallowInterceptTouchEvent(boolean b) {

            }
        });
    }
    public void Reset(){

    }
    public void onStateChange(boolean isPanelOn){
        if(isPanelOn) panel.setVisibility(View.VISIBLE);
        else panel.setVisibility(View.INVISIBLE);
    }
}

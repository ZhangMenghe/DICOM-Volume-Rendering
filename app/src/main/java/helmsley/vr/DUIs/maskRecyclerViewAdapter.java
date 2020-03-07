package helmsley.vr.DUIs;
import android.app.Activity;
import android.content.res.TypedArray;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.List;

import helmsley.vr.R;

public class maskRecyclerViewAdapter extends RecyclerView.Adapter<maskRecyclerViewAdapter.MyView> {
    private final WeakReference<Activity> actRef;
    private final WeakReference<RecyclerView> recyRef;
    private List<String> list;
    private Boolean[] values;
    private int card_num = 0;

    private int norm_bg_color, high_bg_color;

    class MyView extends RecyclerView.ViewHolder {

        TextView textView;

        MyView(View view) {
            super(view);

            textView = (TextView) view.findViewById( R.id.horizontal_card_text);

        }
    }

    maskRecyclerViewAdapter(Activity activity, RecyclerView recyclerView) {
        actRef = new WeakReference<>(activity);
        recyRef = new WeakReference<>(recyclerView);

        norm_bg_color = ContextCompat.getColor(activity, R.color.brightBlue);
        high_bg_color = ContextCompat.getColor(activity, R.color.yellowOrange);

        set_initial_values();
    }

    @Override
    public MyView onCreateViewHolder(ViewGroup parent, int viewType) {
        View card_view = LayoutInflater.from(parent.getContext()).inflate(R.layout.horizontal_item, parent, false);
        setButtonStyle(card_view, card_num);
        card_num++;

        card_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //show/hide volume details
                int item_position = recyRef.get().getChildAdapterPosition(v);
                values[item_position] = !values[item_position];
                setButtonStyle(v, item_position);
            }
        });
        return new MyView(card_view);
    }

    @Override
    public void onBindViewHolder(final MyView holder, final int position) {
        holder.textView.setText(list.get(position));
    }

    @Override
    public int getItemCount() {
        return list.size();
    }
    private void setButtonStyle(View v, int pos){
        if(values[pos]){
            ((CardView) v).setCardBackgroundColor(high_bg_color);
        }else{
            ((CardView) v).setCardBackgroundColor(norm_bg_color);
        }
    }
    private void set_initial_values(){
        list = Arrays.asList(actRef.get().getResources().getStringArray(R.array.masks_list));
        values = new Boolean[list.size()];

        TypedArray tvalues = actRef.get().getResources().obtainTypedArray(R.array.masks_status);
        for(int i=0; i<list.size(); i++){
            values[i] = tvalues.getBoolean(i, false);
        }
    }
    void Reset(){
        set_initial_values();
        for(int i=0; i<recyRef.get().getChildCount(); i++){
            setButtonStyle(recyRef.get().getChildAt(i), i);
        }
    }
}


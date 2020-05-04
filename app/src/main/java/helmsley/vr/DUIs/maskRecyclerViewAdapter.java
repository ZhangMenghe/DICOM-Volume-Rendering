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
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import helmsley.vr.R;

public class maskRecyclerViewAdapter extends RecyclerView.Adapter<maskRecyclerViewAdapter.MyView> {
    private final WeakReference<Activity> actRef;
    private final WeakReference<RecyclerView> recyRef;
    private List<String> item_names;
    private boolean[] values;
    private int card_num = 0;

    private int norm_bg_color, high_bg_color;

    private int mask_num, mask_bits;
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

        item_names = Arrays.asList(actRef.get().getResources().getStringArray(R.array.masks_list));
        values = new boolean[item_names.size()];
        mask_num = item_names.size() - 1;
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
                if(values[item_position]) mask_bits |= 1 << item_position;
                else mask_bits &= ~(1 << item_position);
                JUIInterface.JUIsetMaskBits(mask_num, mask_bits);

            }
        });
        return new MyView(card_view);
    }

    @Override
    public void onBindViewHolder(final MyView holder, final int position) {
        holder.textView.setText(item_names.get(position));
    }

    @Override
    public int getItemCount() {
        return item_names.size();
    }
    private void setButtonStyle(View v, int pos){
        if(values[pos]){
            ((CardView) v).setCardBackgroundColor(high_bg_color);
        }else{
            ((CardView) v).setCardBackgroundColor(norm_bg_color);
        }
    }
    private void set_initial_values(){
        mask_bits = 0;
        for(int i=0; i<item_names.size(); i++)
            mask_bits+= values[i]? (int)Math.pow(2, i) : 0;
        JUIInterface.JUIsetMaskBits(mask_num, mask_bits);
    }
    void Reset(){
        TypedArray tvalues = actRef.get().getResources().obtainTypedArray(R.array.masks_status);
        for(int i=0; i<item_names.size(); i++)
            values[i] = tvalues.getBoolean(i, true);
        tvalues.recycle();
        set_initial_values();
        for(int i=0; i<recyRef.get().getChildCount(); i++){
            setButtonStyle(recyRef.get().getChildAt(i), i);
        }
    }
    void Reset(boolean[] vs){
        if(vs==null || vs.length!=values.length){Reset(); return;}

        values = vs.clone();
        set_initial_values();
        for(int i=0; i<recyRef.get().getChildCount(); i++){
            setButtonStyle(recyRef.get().getChildAt(i), i);
        }
    }
    boolean[] getValues(){return values;}
}


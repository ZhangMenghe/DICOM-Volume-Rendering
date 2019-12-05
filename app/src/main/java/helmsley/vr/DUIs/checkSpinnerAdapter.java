package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class checkSpinnerAdapter extends RecyclerView.Adapter<checkSpinnerAdapter.itemHolder> {
    //config of each card
    private final WeakReference<UIsManager> UIManagerRef;
    private Map<String, Boolean> check_map=new HashMap<>();


    public static class itemHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewName;
        CheckBox checkbox;
        public itemHolder(View view) {
            super(view);
            this.textViewName = (TextView) itemView.findViewById(R.id.checkSpinnerName);
//            this.checkbox = (CheckBox) itemView.findViewById(R.id.checkSpinnerCheckBox);
        }
    }
    public checkSpinnerAdapter(UIsManager manager){
        UIManagerRef = new WeakReference<>(manager);
    }
    // Create new views (invoked by the layout manager)
    @Override
    public checkSpinnerAdapter.itemHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        // create a new view
        View item_view = (View) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.spinner_check_layout, parent, false);
//        CheckBox cb = (CheckBox) item_view.findViewById(R.id.checkSpinnerCheckBox);
//        cb.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                if(((CompoundButton) view).isChecked()){
//                    System.out.println("Checked");
//                } else {
//                    System.out.println("Un-Checked");
//                }
//            }
//        });

        return new itemHolder(item_view);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(itemHolder holder, int position) {
//        holder.textViewName.setText(UIManagerRef.get(check_map.get(position)));
//        datasetInfo info = downloaderReference.get().getAvailableDataset().get(position);
//        holder.textViewDate.setText(info.getDate());
//        holder.textViewPatient.setText(info.getPatientName());
//        //todo:nothing
//        holder.textViewDetail.setText(activityReference.get().getString(R.string.card_data_detail,info.getFolderName(),info.getFileNums()));
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return 0;
//        return downloaderReference.get().getAvailableDataset().size();
    }
}

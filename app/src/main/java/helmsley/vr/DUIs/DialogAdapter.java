package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import helmsley.vr.R;
import helmsley.vr.proto.datasetInfo;

public class DialogAdapter extends RecyclerView.Adapter<DialogAdapter.cardHolder> {
    final static String TAG = "DialogAdapter";
    private ArrayList<datasetInfo> mDataset;
    private final WeakReference<Activity> activityReference;
    private final WeakReference<RecyclerView> recyclerView;

    //config of each card
    public static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewDate;
        TextView textViewPatient;
        TextView textViewDetail;
        public cardHolder(View view) {
            super(view);
            this.textViewDate = (TextView) itemView.findViewById(R.id.textDate);
            this.textViewPatient = (TextView) itemView.findViewById(R.id.textPatientName);
            this.textViewDetail = (TextView) itemView.findViewById(R.id.textFolderName);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public DialogAdapter(Activity activity, RecyclerView recycle_view, ArrayList<datasetInfo> myDataset) {
        mDataset = myDataset;
        activityReference = new WeakReference<Activity>(activity);
        recyclerView = new WeakReference<>(recycle_view);
    }

    // Create new views (invoked by the layout manager)
    @Override
    public DialogAdapter.cardHolder onCreateViewHolder(ViewGroup parent,
                                                         int viewType) {
        // create a new view
        View card_view = (View) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.dataset_cards_layout, parent, false);

        card_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int selectedItemPosition = recyclerView.get().getChildAdapterPosition(v);
                dialogUIs.Download(mDataset.get(selectedItemPosition).getFolderName());
            }
        });
        return new cardHolder(card_view);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(cardHolder holder, int position) {
        //todo:put actual data
        datasetInfo info = mDataset.get(position);
        holder.textViewDate.setText(info.getDate());//("01/01/19");
        holder.textViewPatient.setText(info.getPatientName());//("Larry Smarr");
        holder.textViewDetail.setText(activityReference.get().getString(R.string.card_data_detail,info.getFolderName(),info.getFileNums()));
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
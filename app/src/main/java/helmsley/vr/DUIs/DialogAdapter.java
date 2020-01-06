package helmsley.vr.DUIs;

import android.app.Activity;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.proto.datasetResponse.datasetInfo;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.volumeResponse;

public class DialogAdapter extends RecyclerView.Adapter<DialogAdapter.cardHolder> {
    final static String TAG = "DialogAdapter";
    private final WeakReference<Activity> activityReference;
    private final WeakReference<RecyclerView> recyclerView;
    private final WeakReference<fileTransferClient> downloaderReference;
    private boolean isLocal;

    //config of each card
    public static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewDate;
        TextView textViewPatient;
        TextView textViewDetail;
        ListView lstViewVol;
        public cardHolder(View view) {
            super(view);
            this.textViewDate = (TextView) itemView.findViewById(R.id.textDate);
            this.textViewPatient = (TextView) itemView.findViewById(R.id.textPatientName);
            this.textViewDetail = (TextView) itemView.findViewById(R.id.textFolderName);
            this.lstViewVol = (ListView) itemView.findViewById(R.id.card_list);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public DialogAdapter(Activity activity, RecyclerView recycle_view, fileTransferClient downloader, boolean local) {
        downloaderReference = new WeakReference<>(downloader);
        activityReference = new WeakReference<>(activity);
        recyclerView = new WeakReference<>(recycle_view);
        isLocal = local;
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
                //show/hide volume details
                int selectedItemPosition = recyclerView.get().getChildAdapterPosition(v);
                ViewGroup vp = (ViewGroup)v;
                ListView lv = null;
                for(int index=0; index<vp.getChildCount(); ++index) {
                    View child = vp.getChildAt(index);
                    if(child.getId() == R.id.card_list){
                        lv = (ListView)child;
                        if(!isLocal && lv.getCount() == 0)
                            createListViewContent(lv, downloaderReference.get().getAvailableDataset(isLocal).get(selectedItemPosition).getFolderName());
                        if(child.getVisibility() == View.VISIBLE)child.setVisibility(View.GONE);
                        else child.setVisibility(View.VISIBLE);
                        break;
                    }
                }
            }
        });
        return new cardHolder(card_view);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(cardHolder holder, int position) {
        datasetInfo info = downloaderReference.get().getAvailableDataset(isLocal).get(position);
        holder.textViewDate.setText(info.getDate());
        holder.textViewPatient.setText(info.getPatientName());
        //todo:nothing
        holder.textViewDetail.setText("Details:..");
//        holder.textViewDetail.setText(activityReference.get().getString(R.string.card_data_detail,info.getFolderName(),info.getFileNums()));

        if(isLocal) createListViewContent(holder.lstViewVol, info.getFolderName());

        holder.lstViewVol.setVisibility(View.GONE);
        holder.lstViewVol.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                dialogUIs.RequestVolumeFromDataset(info.getFolderName(), position, isLocal);
            }
        });
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return downloaderReference.get().getAvailableDataset(isLocal).size();
    }

    private void createListViewContent(ListView lv, String ds_name){
        List<volumeResponse.volumeInfo> vol_lst = downloaderReference.get().getAvailableVolumes(ds_name, isLocal);
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : vol_lst)
            volcon_lst.add(activityReference.get().getString(R.string.volume_lst_item, vinfo.getFolderName(), vinfo.getImgWidth(), vinfo.getImgHeight(), vinfo.getFileNums()));
        final ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>
                (activityReference.get(), android.R.layout.simple_list_item_1, volcon_lst);
        //init listview
        lv.setAdapter(arrayAdapter);

        ViewGroup.LayoutParams params = lv.getLayoutParams();
        params.height = lv.getDividerHeight() * arrayAdapter.getCount();

        for (int pos = 0; pos < arrayAdapter.getCount(); pos++) {
            View cv = arrayAdapter.getView(pos, null, lv);
            cv.measure(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            params.height += cv.getMeasuredHeight()
                    + 40 * vol_lst.get(pos).getFolderName().length() / 24;
        }
        lv.setLayoutParams(params);
    }
}
package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.graphics.Bitmap;
import android.support.v7.widget.RecyclerView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.Utils.SwipeDetector;
import helmsley.vr.proto.configResponse;
import helmsley.vr.proto.datasetResponse.datasetInfo;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.volumeResponse;

public class DSCardRecyclerViewAdapter extends RecyclerView.Adapter<DSCardRecyclerViewAdapter.cardHolder> {
    final static String TAG = "DSCardRecyclerViewAdapter";
    private final WeakReference<Activity> actRef;
    private final WeakReference<RecyclerView> recyclerView;
    private final WeakReference<fileTransferClient> downloaderReference;
    private final WeakReference<dialogUIs> dUIRef;

    private ArrayAdapter<String> contentAdapter;
    private LinkedHashMap<String, List<volumeResponse.volumeInfo>> cached_volumeinfo;
    private dialogUIs.DownloadDialogType infotype_;

    private AlertDialog preview_dialog = null;
    private ImageView preview_img_view;
    private TextView title_tex_view, content_tex_view;
    private static volumeResponse.volumeInfo sel_vol_info;
    private static String sel_ds_name;
    private static boolean sel_is_local;

    //config of each card
    static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewDate;
        TextView textViewPatient;
        TextView textViewDetail;
        ListView lstViewVol;
        TextView textContent;
        cardHolder(View view) {
            super(view);
            this.textViewDate = (TextView) itemView.findViewById(R.id.textDate);
            this.textViewPatient = (TextView) itemView.findViewById(R.id.textPatientName);
            this.textViewDetail = (TextView) itemView.findViewById(R.id.textFolderName);
            this.lstViewVol = (ListView) itemView.findViewById(R.id.card_list);
            this.textContent = (TextView) itemView.findViewById(R.id.card_detail);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    DSCardRecyclerViewAdapter(Activity activity, RecyclerView recycle_view, fileTransferClient downloader, dialogUIs dui, dialogUIs.DownloadDialogType type) {
        downloaderReference = new WeakReference<>(downloader);
        actRef = new WeakReference<>(activity);
        recyclerView = new WeakReference<>(recycle_view);
        dUIRef = new WeakReference<>(dui);
        infotype_ = type;
        cached_volumeinfo = new LinkedHashMap<>();
    }

    // Create new views (invoked by the layout manager)
    @Override
    public DSCardRecyclerViewAdapter.cardHolder onCreateViewHolder(ViewGroup parent,
                                                         int viewType) {
        // create a new view
        View card_view = (View) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.dataset_cards_layout, parent, false);
        if(infotype_ == dialogUIs.DownloadDialogType.CONFIGS)
            card_view.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    ViewGroup vp = (ViewGroup)v;
                    for(int index=0; index<vp.getChildCount(); ++index) {
                        View child = ((ViewGroup) v).getChildAt(index);
                        if(child.getId() == R.id.card_detail){
                            if(child.getVisibility() == View.VISIBLE)child.setVisibility(View.GONE);
                            else child.setVisibility(View.VISIBLE);
                            break;
                        }
                    }
                }
            });
        else
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
                        boolean isLocal = (infotype_ == dialogUIs.DownloadDialogType.DATA_LOCAL);
                        if(!isLocal && lv.getCount() == 0)
                            setup_single_card_content_list(lv, downloaderReference.get().getAvailableDataset(isLocal).get(selectedItemPosition).getFolderName(), false);

                        if(child.getVisibility() == View.VISIBLE)child.setVisibility(View.GONE);
                        else child.setVisibility(View.VISIBLE);
                        break;
                    }
                }
            }
        });
        return new cardHolder(card_view);
    }

    private void onBindViewHolderConfig(cardHolder holder, int position) {
        configResponse.configInfo info = downloaderReference.get().getAvailableConfigFiles().get(position);
        holder.textViewPatient.setText(info.getFileName());
        holder.textContent.setText(info.getContent());
//        holder.textContent.setVisibility(View.GONE);
        holder.textContent.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dUIRef.get().LoadConfig(info.getContent());
            }
        });
    }
    private void setup_preview_dialog(){
        DisplayMetrics displayMetrics = new DisplayMetrics();
        actRef.get().getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

        //setup dialog
        final ViewGroup parent_view = (ViewGroup)actRef.get().findViewById(R.id.parentPanel);
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(actRef.get());
        final View dialogView = LayoutInflater.from(actRef.get())
                .inflate(R.layout.preview_dialog_layout, parent_view, false);

        layoutDialog_builder.setTitle(actRef.get().getString(R.string.preview_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);
        preview_dialog = layoutDialog_builder.create();
        preview_dialog.setCanceledOnTouchOutside(false);
        preview_dialog.getWindow().setLayout((int)(displayMetrics.widthPixels * 0.8), (int)(displayMetrics.heightPixels * 0.8));
        preview_img_view = dialogView.findViewById(R.id.pre_img);
        preview_img_view.setScaleType(ImageView.ScaleType.FIT_CENTER);
        title_tex_view = dialogView.findViewById(R.id.pre_name);
        content_tex_view = dialogView.findViewById(R.id.pre_content);
        Button dismiss_btn = dialogView.findViewById(R.id.pre_dismiss_btn);
        dismiss_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                preview_dialog.dismiss();
            }
        });
        Button import_btn = dialogView.findViewById(R.id.pre_import_btn);
        import_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogUIs.onDownloadingUI(sel_ds_name, sel_is_local);
                //get the information ready here
                List<Integer> dims = sel_vol_info.getDimsList();
                JNIInterface.JNIsendDataPrepare(dims.get(1), dims.get(0), dims.get(2), sel_vol_info.getVolumeLocRange(), sel_vol_info.getScores().getVolScore(2)>0);
                downloaderReference.get().Download(sel_ds_name, sel_vol_info);
                preview_dialog.dismiss();
            }
        });
    }
        // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(cardHolder holder, int position) {
        if(infotype_ == dialogUIs.DownloadDialogType.CONFIGS){onBindViewHolderConfig(holder, position);return;}
        boolean isLocal = (infotype_ == dialogUIs.DownloadDialogType.DATA_LOCAL);

        datasetInfo info = downloaderReference.get().getAvailableDataset(isLocal).get(position);
        holder.textViewDate.setText(info.getDate());
        holder.textViewPatient.setText(info.getPatientName());
        holder.textViewDetail.setText(info.getFolderName());
        holder.textViewDetail.setTextSize(9);

        if(isLocal)setup_single_card_content_list(holder.lstViewVol, info.getFolderName(), true);

        holder.lstViewVol.setVisibility(View.GONE);

        //Perform selection of a volume
        holder.lstViewVol.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                //load data from local / remote
                sel_ds_name = info.getFolderName();
                sel_is_local = isLocal;
                fileTransferClient loader = downloaderReference.get();
//                sel_vol_info = loader.getAvailableVolumes(sel_ds_name, isLocal).get(position);
                sel_vol_info = cached_volumeinfo.get(sel_ds_name).get(position);

                if(preview_dialog == null) setup_preview_dialog();

                int height=sel_vol_info.getDims(0);
                int width = sel_vol_info.getDims(1);

                // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
                Bitmap renderBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ALPHA_8);
                byte[]data = sel_vol_info.getSampleImg().toByteArray();
                for(int i=0;i<data.length;i++)
                    data[i] = (byte)(255 - (int)data[i]);

                ByteBuffer byteBuffer = ByteBuffer.wrap(data);
                renderBitmap.copyPixelsFromBuffer(byteBuffer);

                preview_img_view.setImageBitmap(renderBitmap);
                title_tex_view.setText(actRef.get().getString(R.string.preview_text, sel_vol_info.getFolderName()));
                //name
                String content = "Rank: " + sel_vol_info.getScores().getRankId() + "\n";
                content +="Ranking score: "+ sel_vol_info.getScores().getRankScore() + "\n"
                        + "Tags Score: " + sel_vol_info.getScores().getVolScore(1) + "\n";
                content_tex_view.setText(content);
                preview_dialog.show();
            }
        });

        //Perform "more" options with a fling gesture
        final class lstSwipeDetector extends SwipeDetector{
            private int current_id = -1;
            protected void onSwipeRightToLeft(View v, int x, int y){
                //todo: some notification about the operation
                //perform local deletion
                int pos = holder.lstViewVol.pointToPosition(x,y);
                if(pos == current_id) return;
                current_id = pos;
                if(downloaderReference.get().deleteLocalData(info.getFolderName(), pos))
                    dialogUIs.NotifyChanges();
                current_id = -1;
            }
            protected void onSwipeLeftToRight(View v){
                Log.i(TAG, "Swipe Left to Right");
            }
        }
        holder.lstViewVol.setOnTouchListener(new lstSwipeDetector());
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        if(infotype_ == dialogUIs.DownloadDialogType.CONFIGS)return downloaderReference.get().getAvailableConfigFiles().size();
        boolean isLocal = (infotype_ == dialogUIs.DownloadDialogType.DATA_LOCAL);
        return downloaderReference.get().getAvailableDataset(isLocal).size();
    }

    void onContentChange(){contentAdapter.notifyDataSetChanged();}

    private void setup_single_card_content_list(ListView lv, String ds_name, boolean isLocal){
        List<volumeResponse.volumeInfo> vol_lst = downloaderReference.get().getAvailableVolumes(ds_name, isLocal);
        cached_volumeinfo.put(ds_name, vol_lst);
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : vol_lst){
            List<Integer> dims = vinfo.getDimsList();
            volcon_lst.add(actRef.get().getString(
                    R.string.volume_lst_item, vinfo.getFolderName(), dims.get(1), dims.get(0), dims.get(2))
                    +(vinfo.getScores().getVolScore(2)>0?"\n===>>With Mask<<===":""));
        }

        contentAdapter = new ArrayAdapter<>(actRef.get(), android.R.layout.simple_list_item_1, volcon_lst);

        //init listview
        lv.setAdapter(contentAdapter);

        ViewGroup.LayoutParams params = lv.getLayoutParams();
        params.height = lv.getDividerHeight() * contentAdapter.getCount();

        for (int pos = 0; pos < contentAdapter.getCount(); pos++) {
            View cv = contentAdapter.getView(pos, null, lv);
            cv.measure(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            params.height += cv.getMeasuredHeight()
                    + 40 * vol_lst.get(pos).getFolderName().length() / 24;
        }
        lv.setLayoutParams(params);
    }
}
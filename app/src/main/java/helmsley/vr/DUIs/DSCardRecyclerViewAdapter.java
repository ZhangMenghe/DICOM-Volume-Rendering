package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.support.v7.widget.RecyclerView;
import android.util.ArraySet;
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
import android.widget.Spinner;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

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
    private static WeakReference<DSCardRecyclerViewAdapter> selfReference;
    private final WeakReference<RecyclerView> recyclerView;
    private final WeakReference<fileTransferClient> downloaderReference;
    private final WeakReference<dialogUIs> dUIRef;

    private static Map<String, ArrayAdapter<String>> contentAdapters;
    private LinkedHashMap<String, List<volumeResponse.volumeInfo>> cached_volumeinfo;
    private dialogUIs.DownloadDialogType infotype_;

    private AlertDialog preview_dialog = null;
    private ImageView preview_img_view;
    private Button preview_delete_btn;
    private TextView title_tex_view, content_tex_view;
    private static volumeResponse.volumeInfo sel_vol_info;
    private static String sel_ds_name;
    private static boolean sel_is_local;
    private final static List<String> sort_keys = new ArrayList<>(Arrays.asList("HELM Grade", "Name", "Mean", "Range", "SNR"));
    private final static int[] sort_keys_ids = {-1, -1, 0,1,6};
    private static Set<String> dirty_dsname= new ArraySet<>();
    private Map<String, View> sort_view_map = new LinkedHashMap<>();

    //config of each card
    static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewDate;
        TextView textViewPatient;
        TextView textViewDetail;
        ListView lstViewVol;
        TextView textContent;
        Spinner sortSpinner;
        cardHolder(View view) {
            super(view);
            this.textViewDate = (TextView) itemView.findViewById(R.id.textDate);
            this.textViewPatient = (TextView) itemView.findViewById(R.id.textPatientName);
            this.textViewDetail = (TextView) itemView.findViewById(R.id.textFolderName);
            this.lstViewVol = (ListView) itemView.findViewById(R.id.card_list);
            this.textContent = (TextView) itemView.findViewById(R.id.card_detail);
            this.sortSpinner = (Spinner) itemView.findViewById(R.id.sort_key_spinner);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    DSCardRecyclerViewAdapter(Activity activity, RecyclerView recycle_view, fileTransferClient downloader, dialogUIs dui, dialogUIs.DownloadDialogType type) {
        downloaderReference = new WeakReference<>(downloader);
        selfReference = new WeakReference<>(this);
        actRef = new WeakReference<>(activity);
        recyclerView = new WeakReference<>(recycle_view);
        dUIRef = new WeakReference<>(dui);
        infotype_ = type;
        cached_volumeinfo = new LinkedHashMap<>();
        contentAdapters = new LinkedHashMap<>();
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
                //show/hide list view: volume details
                int selectedItemPosition = recyclerView.get().getChildAdapterPosition(v);
                ListView lst_view = (ListView)v.findViewById(R.id.card_list);
                sel_is_local = (infotype_ == dialogUIs.DownloadDialogType.DATA_LOCAL);
                sel_ds_name = downloaderReference.get().getAvailableDataset(sel_is_local).get(selectedItemPosition).getFolderName();

                if(!sel_is_local && lst_view.getCount() == 0)
                    setup_single_card_content_list(lst_view, sel_ds_name, false);

                if(lst_view.getVisibility() == View.VISIBLE)lst_view.setVisibility(View.GONE);
                else lst_view.setVisibility(View.VISIBLE);

                //show/hide sort view
                View sort_view = v.findViewById(R.id.card_sort_layout);
                if(sort_view.getVisibility() == View.VISIBLE) sort_view.setVisibility(View.GONE);
                else sort_view.setVisibility(View.VISIBLE);
                sort_view_map.put(sel_ds_name, sort_view);
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
        preview_delete_btn = dialogView.findViewById(R.id.pre_delete_btn);
        preview_delete_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(downloaderReference.get().deleteLocalData(sel_ds_name, sel_vol_info)) DirtyCache(sel_ds_name);
                preview_dialog.dismiss();
            }
        });
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
                //get the information ready here
                List<Integer> dims = sel_vol_info.getDimsList();
                JNIInterface.JNIsendDataPrepare(dims.get(1), dims.get(0), dims.get(2), sel_vol_info.getVolumeLocRange(), sel_vol_info.getWithMask());
                if(downloaderReference.get().Download(sel_ds_name, sel_vol_info))dialogUIs.onDownloadingUI(sel_ds_name, sel_is_local);
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
//                fileTransferClient loader = downloaderReference.get();
//                sel_vol_info = loader.getAvailableVolumes(sel_ds_name, isLocal).get(position);
                if(!dirty_dsname.isEmpty()){
                    for(String name:dirty_dsname){
                        List<volumeResponse.volumeInfo> vol_lst = downloaderReference.get().getAvailableVolumes(name, sel_is_local);
                        if(vol_lst!=null && !vol_lst.isEmpty()) cached_volumeinfo.put(name, vol_lst);
                    }
                    dirty_dsname.clear();
                }
                //todo:debug, select from remote, the second time after reordering
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
                if(isLocal)preview_delete_btn.setVisibility(View.VISIBLE);
                else preview_delete_btn.setVisibility(View.GONE);
                //name
                String content = "Rank: " + sel_vol_info.getScores().getRankId() + "\n";
                content +="Ranking score: "+ sel_vol_info.getScores().getRankScore() + "\n"
                        + "Tags Score: " + sel_vol_info.getScores().getVolScore(1) + "\n";
                content_tex_view.setText(content);
                preview_dialog.show();
            }
        });

        //order matter!
        sortListAdapter adp = new sortListAdapter(actRef.get(),this, sort_keys, info.getFolderName());
        holder.sortSpinner.setAdapter(adp);
    }
    public static void DirtyCache(String dsname){
        dirty_dsname.add(dsname);
        try{
            selfReference.get().notifyDataSetChanged();
            for(View sort_view: selfReference.get().sort_view_map.values())
                sort_view.setVisibility(View.GONE);
            contentAdapters.get(dsname).notifyDataSetChanged();
        }catch (NullPointerException e){
        }

    }
    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        if(infotype_ == dialogUIs.DownloadDialogType.CONFIGS)return downloaderReference.get().getAvailableConfigFiles().size();
        boolean isLocal = (infotype_ == dialogUIs.DownloadDialogType.DATA_LOCAL);
        return downloaderReference.get().getAvailableDataset(isLocal).size();
    }

    private void setup_single_card_content_list(ListView lv, String ds_name, boolean isLocal){
        List<volumeResponse.volumeInfo> vol_lst = downloaderReference.get().getAvailableVolumes(ds_name, isLocal);
        if(vol_lst.isEmpty()) return;
        cached_volumeinfo.put(ds_name, vol_lst);

        //setup card info
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : cached_volumeinfo.get(ds_name)){
            List<Integer> dims = vinfo.getDimsList();
            volcon_lst.add(actRef.get().getString(
                    R.string.volume_lst_item, vinfo.getFolderName(), dims.get(1), dims.get(0), dims.get(2))
                    +(vinfo.getWithMask()?"\n===>>With Mask<<===":""));
        }

        ArrayAdapter<String> contentAdapter = new ArrayAdapter<>(actRef.get(), android.R.layout.simple_list_item_1, volcon_lst);

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
        contentAdapters.put(ds_name, contentAdapter);
    }
    private void ReorderVolumeList(String key){
        List<volumeResponse.volumeInfo> info_lst = cached_volumeinfo.get(sel_ds_name);//downloaderReference.get().getAvailableVolumes(sel_ds_name, sel_is_local);

        int kid = sort_keys_ids[sort_keys.indexOf(key)];
        if(kid > 0)
        Collections.sort(info_lst, new Comparator<volumeResponse.volumeInfo>() {
            @Override
            public int compare(volumeResponse.volumeInfo u1, volumeResponse.volumeInfo u2) {
                float r1 = u1.getScores().getRawScore(kid);
                float r2 = u2.getScores().getRawScore(kid);
                return Float.compare(r2, r1);
            }
        });
        else{
            if(key.equals("Name")){
                Collections.sort(info_lst, new Comparator<volumeResponse.volumeInfo>() {
                    @Override
                    public int compare(volumeResponse.volumeInfo u1, volumeResponse.volumeInfo u2) {
                        try{
                            float f1 = Float.parseFloat(u1.getFolderName().split("_")[0]);
                            return Float.compare(f1, Float.parseFloat(u2.getFolderName().split("_")[0]));
                        }catch (NumberFormatException e1){
                            try{
                                float f12 = Float.parseFloat(u1.getFolderName().split("_")[1]);
                                return Float.compare(f12,Float.parseFloat(u2.getFolderName().split("_")[1]));
                            }catch (NumberFormatException e2){
                                return Character.compare(u1.getFolderName().charAt(0), u2.getFolderName().charAt(0));
                            }
                        }
                    }
                });
            }else{
                Collections.sort(info_lst, new Comparator<volumeResponse.volumeInfo>() {
                    @Override
                    public int compare(volumeResponse.volumeInfo u1, volumeResponse.volumeInfo u2) {
                        float r1 = u1.getScores().getRankScore();
                        float r2 = u2.getScores().getRankScore();
                        return Float.compare(r2,r1);
                    }
                });
            }

        }

        cached_volumeinfo.put(sel_ds_name, info_lst);

        //setup card info
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : cached_volumeinfo.get(sel_ds_name)){
            List<Integer> dims = vinfo.getDimsList();
            volcon_lst.add(actRef.get().getString(
                    R.string.volume_lst_item, vinfo.getFolderName(), dims.get(1), dims.get(0), dims.get(2))
                    +(vinfo.getWithMask()?"\n===>>With Mask<<===":""));
        }
        if(!contentAdapters.containsKey(sel_ds_name))
            contentAdapters.put(sel_ds_name, new ArrayAdapter<>(actRef.get(), android.R.layout.simple_list_item_1, volcon_lst));
        else{
            contentAdapters.get(sel_ds_name).clear();
            contentAdapters.get(sel_ds_name).addAll(volcon_lst);
            contentAdapters.get(sel_ds_name).notifyDataSetChanged();
        }
    }
    private static class sortListAdapter extends textSimpleListAdapter{
        int current_id = 0;
        String ds_name_;
        private final WeakReference<DSCardRecyclerViewAdapter> parentRef;
        sortListAdapter(Context context, DSCardRecyclerViewAdapter parent, List<String> arrs, String ds_name){
            super(context, arrs);
            ds_name_ = ds_name;
            parentRef = new WeakReference<>(parent);
            setTitleById(current_id);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            if(current_id == id) return;
            current_id = id;
            sel_ds_name = ds_name_;
            parentRef.get().ReorderVolumeList(item_names.get(id));
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            int nid = item_names.indexOf(title);
            if(nid == current_id) return;
            current_id = nid;
            sel_ds_name = ds_name_;
            parentRef.get().ReorderVolumeList(title);
        }
        void onItemClick(int position){
            if(current_id == position) return;
            current_id = position;
            sel_ds_name = ds_name_;
            parentRef.get().ReorderVolumeList(item_names.get(position));
        }
    }
}
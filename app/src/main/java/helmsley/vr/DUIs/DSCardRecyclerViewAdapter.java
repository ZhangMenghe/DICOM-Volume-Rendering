package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.support.v7.widget.RecyclerView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
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
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.proto.datasetResponse.datasetInfo;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.volumeResponse;

import static helmsley.vr.DUIs.dialogUIs.DownloadDialogType.*;

public class DSCardRecyclerViewAdapter extends RecyclerView.Adapter<DSCardRecyclerViewAdapter.cardHolder> {
    //config of each card
    static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textViewDate;
        TextView textViewPatient;
        TextView textViewDetail;
        ListView lstViewVol;
        TextView textContent;
        Spinner sortSpinner;
        View sortView;
        cardHolder(View view) {
            super(view);
            this.textViewDate = (TextView) itemView.findViewById(R.id.textDate);
            this.textViewPatient = (TextView) itemView.findViewById(R.id.textPatientName);
            this.textViewDetail = (TextView) itemView.findViewById(R.id.textFolderName);
            this.lstViewVol = (ListView) itemView.findViewById(R.id.card_list);
            this.textContent = (TextView) itemView.findViewById(R.id.card_detail);
            this.sortSpinner = (Spinner) itemView.findViewById(R.id.sort_key_spinner);
            this.sortView = itemView.findViewById(R.id.card_sort_layout);
        }
    }

    final static String TAG = "DSCardRecyclerViewAdapter";
    private final WeakReference<Activity> actRef;
    private final WeakReference<RecyclerView> recyclerView;
    private final WeakReference<fileTransferClient> downloaderRef;
    private final WeakReference<dialogUIs> duiRef;
    private final WeakReference<DSCardRecyclerViewAdapter> selfRef;
    private final boolean islocal_;

    //adapter of listView content
    private Map<String, ArrayAdapter<String>> contentAdapters;
    //cache remote, keep it null for local ds
    private LinkedHashMap<String, List<volumeResponse.volumeInfo>> cached_volumeinfo = null;

    //uis for preview
    private AlertDialog preview_dialog = null;
    private ImageView preview_img_view;
    private Button preview_delete_btn;
    private TextView title_tex_view, content_tex_view;
    private static int PREVIEW_IMG_HEIGHT;

    //"selected info"
    private volumeResponse.volumeInfo sel_vol_info;
    private String sel_ds_name;

    //sorting stuff
    private final static List<String> sort_keys = new ArrayList<>(Arrays.asList("HELM Grade", "Name", "Mean", "Range", "SNR"));
    private final static int[] sort_keys_ids = {-1, -1, 0, 1, 6};
//    private static Set<String> dirty_dsname= new ArraySet<>();
    private Map<String, View> sort_view_map = new LinkedHashMap<>();
//    private Map<String, View> lst_view_map = new LinkedHashMap<>();
    private ProgressTask p_tsk;

    DSCardRecyclerViewAdapter(Activity activity, RecyclerView recycle_view, fileTransferClient downloader, dialogUIs dui, dialogUIs.DownloadDialogType type) {
        downloaderRef = new WeakReference<>(downloader);
        actRef = new WeakReference<>(activity);
        recyclerView = new WeakReference<>(recycle_view);
        duiRef = new WeakReference<>(dui);
        islocal_ = (type == DATA_LOCAL);
        if(!islocal_) cached_volumeinfo = new LinkedHashMap<>();

        contentAdapters = new LinkedHashMap<>();
        TypedValue typedValue = new TypedValue();
        activity.getResources().getValue(R.dimen.preview_img_height, typedValue, true);
        PREVIEW_IMG_HEIGHT = (int)typedValue.getFloat();
        selfRef = new WeakReference<>(this);
    }
    @Override
    public DSCardRecyclerViewAdapter.cardHolder onCreateViewHolder(ViewGroup parent,
                                                         int viewType) {
        // create a new view
        View card_view = (View) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.dataset_cards_layout, parent, false);

        card_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int selectedItemPosition = recyclerView.get().getChildAdapterPosition(v);
                sel_ds_name = downloaderRef.get().getAvailableDataset(islocal_).get(selectedItemPosition).getFolderName();
                if(islocal_ || ((ListView)v.findViewById(R.id.card_list)).getCount() != 0) {
                    on_click_ds_card(v);
                }else
                    new ProgressTask(selfRef.get(),v).execute();
            }
        });
        return new cardHolder(card_view);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(cardHolder holder, int position) {
        datasetInfo info = downloaderRef.get().getAvailableDataset(islocal_).get(position);
        holder.textViewDate.setText(info.getDate());
        holder.textViewPatient.setText(info.getPatientName());
        holder.textViewDetail.setText(info.getFolderName());
        holder.textViewDetail.setTextSize(9);
        sort_view_map.put(info.getFolderName(), holder.sortView);

        if(islocal_) setup_single_card_content_list(holder.lstViewVol, info.getFolderName(), true);

        holder.lstViewVol.setVisibility(View.GONE);
        holder.sortView.setVisibility(View.GONE);

        //Perform selection of a volume
        holder.lstViewVol.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                //load data from local / remote
                sel_ds_name = info.getFolderName();
                if(islocal_)sel_vol_info = downloaderRef.get().getLocalVolumeFromDSAt(sel_ds_name, position);
                else sel_vol_info = cached_volumeinfo.get(sel_ds_name).get(position);

                //setup preview and contents
                if(preview_dialog == null) setup_preview_dialog();

                int height = sel_vol_info.getDims(0);
                int width  = sel_vol_info.getDims(1);

                // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
                Bitmap renderBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ALPHA_8);
                byte[]data = sel_vol_info.getSampleImg().toByteArray();
                for(int i=0;i<data.length;i++)
                    data[i] = (byte)(255 - (int)data[i]);
                ByteBuffer byteBuffer = ByteBuffer.wrap(data);
                renderBitmap.copyPixelsFromBuffer(byteBuffer);
                if(height == PREVIEW_IMG_HEIGHT){
                    preview_img_view.setImageBitmap(renderBitmap);
                }else{
                    Bitmap bMapScaled = Bitmap.createScaledBitmap(renderBitmap, (int)(width/height * PREVIEW_IMG_HEIGHT), PREVIEW_IMG_HEIGHT, true);
                    preview_img_view.setImageBitmap(bMapScaled);
                }
                title_tex_view.setText(actRef.get().getString(R.string.preview_text, sel_vol_info.getFolderName()));
                //name
                String content = "HELM Rank: " + sel_vol_info.getScores().getRankId() + "\n";
                content +="HELM Rank score: "+ sel_vol_info.getScores().getRankScore() + "\n"
                        + "Dimensions: " + height + "x"+width+"x"+sel_vol_info.getDims(2) + '\n';
                content_tex_view.setText(content);
                preview_dialog.show();
            }
        });

        //order matter!
        sortListAdapter adp = new sortListAdapter(actRef.get(),this, sort_keys, info.getFolderName());
        holder.sortSpinner.setAdapter(adp);
        adp.setTitleById(0);
    }
    @Override
    public int getItemCount() {
        return downloaderRef.get().getAvailableDataset(islocal_).size();
    }

    private void setup_single_card_content_list(ListView lv, String ds_name, boolean isLocal){
        //todo:on processing ui?
        List<volumeResponse.volumeInfo> vol_lst = downloaderRef.get().getAvailableVolumes(ds_name, isLocal);
        if(vol_lst.isEmpty()) return;
        if(!isLocal) cached_volumeinfo.put(ds_name, vol_lst);

        //setup card info
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : vol_lst){
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
        if(islocal_){
            preview_delete_btn.setVisibility(View.VISIBLE);
            preview_delete_btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (downloaderRef.get().deleteLocalData(sel_ds_name, sel_vol_info))
                        if(!updateLstContent(sel_ds_name, downloaderRef.get().getAvailableVolumes(sel_ds_name, true)))
                            updateCardContent();
                    preview_dialog.dismiss();
                }
            });
        }else preview_delete_btn.setVisibility(View.GONE);

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
                List<Float> spacing = sel_vol_info.getResolutionList();
//                Log.e(TAG, "====onClick: "+ spacing.get(0)*dims.get(0) + " ====" +spacing.get(1)*dims.get(1) + "==="+sel_vol_info.getVolumeLocRange() );
                JNIInterface.JNIsendDataPrepare(
                        sel_ds_name + "/"+ sel_vol_info.getFolderName(),
                        dims.get(0), dims.get(1), dims.get(2),
                        spacing.get(0)*dims.get(0), spacing.get(1)*dims.get(1), sel_vol_info.getVolumeLocRange(),
                        sel_vol_info.getWithMask());
                
                if(downloaderRef.get().Download(sel_ds_name, sel_vol_info, islocal_))
                    duiRef.get().onDownloadingUI(islocal_);
                else
                    Toast.makeText(actRef.get(), "Fail to load Data", Toast.LENGTH_LONG);
                preview_dialog.dismiss();
            }
        });
    }
    private void ReorderVolumeList(String ds_name, String key){
        sel_ds_name = ds_name;
        List<volumeResponse.volumeInfo> info_lst;
        if(islocal_) info_lst = downloaderRef.get().getAvailableVolumes(sel_ds_name, true);
        else info_lst = cached_volumeinfo.get(sel_ds_name);
        if(info_lst==null) return;
//        for(volumeResponse.volumeInfo vinfo:info_lst)
//            Log.e(TAG, "===before: " + vinfo.getFolderName() );

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

        if(islocal_)downloaderRef.get().setLocalVolumes(sel_ds_name, info_lst);
        else cached_volumeinfo.put(sel_ds_name, info_lst);
        updateLstContent(sel_ds_name, info_lst);
    }
    void updateCardContent(){
        notifyDataSetChanged();
        for(View sort_view: sort_view_map.values())
            sort_view.setVisibility(View.GONE);
    }
    boolean updateLstContent(String ds_name, List<volumeResponse.volumeInfo> info_lst){
        if(info_lst==null || info_lst.isEmpty()) return false;
        //setup card info
        ArrayList<String> volcon_lst = new ArrayList<>();
        for (volumeResponse.volumeInfo vinfo : info_lst){
            List<Integer> dims = vinfo.getDimsList();
            volcon_lst.add(actRef.get().getString(
                    R.string.volume_lst_item, vinfo.getFolderName(), dims.get(1), dims.get(0), dims.get(2))
                    +(vinfo.getWithMask()?"\n===>>With Mask<<===":""));
        }
        if(!contentAdapters.containsKey(ds_name))
            contentAdapters.put(ds_name, new ArrayAdapter<>(actRef.get(), android.R.layout.simple_list_item_1, volcon_lst));
        else{
            contentAdapters.get(ds_name).clear();
            contentAdapters.get(ds_name).addAll(volcon_lst);
        }
        contentAdapters.get(ds_name).notifyDataSetChanged();
        return true;
    }
    private void on_click_ds_card(View v){
        ListView lst_view = (ListView)v.findViewById(R.id.card_list);
        //setup remote card content
        if(!islocal_ && lst_view.getCount() == 0)
            setup_single_card_content_list(lst_view, sel_ds_name, false);

        //show/hide list view: volume details
        if(lst_view.getVisibility() == View.VISIBLE)lst_view.setVisibility(View.GONE);
        else lst_view.setVisibility(View.VISIBLE);
        //show/hide sort view
        View sort_view;
        try{
            sort_view = sort_view_map.get(sel_ds_name);
        }catch (NullPointerException e){
            sort_view = v.findViewById(R.id.card_sort_layout);
            sort_view_map.put(sel_ds_name, sort_view);
        }
        if(sort_view!=null){
            sort_view.setVisibility(lst_view.getVisibility());
//            if(sort_view.getVisibility() == View.VISIBLE) sort_view.setVisibility(View.GONE);
//            else sort_view.setVisibility(View.VISIBLE);
        }
    }
    private static class sortListAdapter extends textSimpleListAdapter{
        int current_id = -1;
        String ds_name_;
        private final WeakReference<DSCardRecyclerViewAdapter> parentRef;
        sortListAdapter(Context context, DSCardRecyclerViewAdapter parent, List<String> arrs, String ds_name){
            super(context, arrs);
            ds_name_ = ds_name;
            parentRef = new WeakReference<>(parent);
        }
        void setTitleById(int id){
            super.setTitleById(id);
            if(current_id == id) return;
            current_id = id;
            parentRef.get().ReorderVolumeList(ds_name_,item_names.get(id));
        }
        void setTitleByText(String title) {
            super.setTitleByText(title);
            int nid = item_names.indexOf(title);
            if(nid == current_id) return;
            current_id = nid;
            parentRef.get().ReorderVolumeList(ds_name_,title);
        }
        void onItemClick(int position){
            if(current_id == position) return;
            current_id = position;
            parentRef.get().ReorderVolumeList(ds_name_,item_names.get(position));
        }
    }


    private static class ProgressTask extends AsyncTask<Void, Void, Void> {
        private final WeakReference<DSCardRecyclerViewAdapter> parentRef;
        private final WeakReference<View> vRef;
        ProgressTask(DSCardRecyclerViewAdapter adp, View v_){
            parentRef = new WeakReference<>(adp);
            vRef = new WeakReference<>(v_);
        }
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            parentRef.get().duiRef.get().showProgress();
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            parentRef.get().on_click_ds_card(vRef.get());
            parentRef.get().duiRef.get().hideProgress();
        }

        @Override
        protected Void doInBackground(Void... params) {
            return null;
        }
    }
}
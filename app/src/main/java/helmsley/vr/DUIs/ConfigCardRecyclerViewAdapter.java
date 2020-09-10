package helmsley.vr.DUIs;

import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.proto.Request;
import helmsley.vr.proto.commonResponse;
import helmsley.vr.proto.configResponse;
import helmsley.vr.proto.rpcManager;

public class ConfigCardRecyclerViewAdapter extends RecyclerView.Adapter<ConfigCardRecyclerViewAdapter.cardHolder> {
    //config of each card
    static class cardHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        TextView textView_name;
        ListView lstView_content;
        TextView textView_content;
        cardHolder(View view) {
            super(view);
            this.textView_name = (TextView) itemView.findViewById(R.id.text_title);
            this.lstView_content = (ListView) itemView.findViewById(R.id.card_list);
            this.textView_content = (TextView)itemView.findViewById(R.id.card_detail);
        }
    }

    private final WeakReference<rpcManager> rpcRef;
    private final WeakReference<dialogUIs> dUIRef;

    private List<configResponse.configInfo> available_config_files;
    private boolean config_dirty;

    ConfigCardRecyclerViewAdapter(rpcManager manager, dialogUIs dui){
        rpcRef = new WeakReference<>(manager);
        dUIRef = new WeakReference<>(dui);
        config_dirty = true;
    }
    void SetupContents(){
//        Request req = Request.newBuilder().setClientId(rpcManager.CLIENT_ID).build();
//        available_config_files = rpcManager.data_stub.getAvailableConfigs(req).getConfigsList();
//        config_dirty = false;
    }
    @NonNull
    @Override
    public cardHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        // create a new view
        View card_view = (View) LayoutInflater.from(viewGroup.getContext())
                .inflate(R.layout.config_card_layout, viewGroup, false);
        card_view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                View dv = v.findViewById(R.id.card_detail);

                if(dv.getVisibility() == View.VISIBLE)dv.setVisibility(View.GONE);
                else dv.setVisibility(View.VISIBLE);
            }
        });
        return new cardHolder(card_view);
    }

    @Override
    public void onBindViewHolder(@NonNull cardHolder cardHolder, int i) {
        configResponse.configInfo info = getAvailableConfigFiles().get(i);
        cardHolder.textView_name.setText(info.getFileName());
        cardHolder.textView_content.setText(info.getContent());
        cardHolder.textView_content.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dUIRef.get().LoadConfig(info.getContent());
            }
        });
    }

    @Override
    public int getItemCount() {
        return getAvailableConfigFiles().size();
    }

    private List<configResponse.configInfo> getAvailableConfigFiles(){
        if(config_dirty){
            try{
                Request req = Request.newBuilder().setClientId(rpcManager.CLIENT_ID).build();
                available_config_files = rpcManager.data_stub.getAvailableConfigs(req).getConfigsList();
                config_dirty = false;
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
        return available_config_files;
    }

    void ExportConfig(String content){
        if (content == null) return;
        Request req = Request.newBuilder().setClientId(rpcManager.CLIENT_ID).setReqMsg(content).build();

        commonResponse res = rpcManager.data_stub.exportConfigs(req);
        config_dirty = true;
    }
}


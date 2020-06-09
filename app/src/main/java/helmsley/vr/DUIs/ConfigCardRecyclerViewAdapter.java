package helmsley.vr.DUIs;

import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.TextView;
import java.lang.ref.WeakReference;
import helmsley.vr.R;
import helmsley.vr.proto.configResponse;
import helmsley.vr.proto.fileTransferClient;

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

    private final WeakReference<fileTransferClient> downloadRef;
    private final WeakReference<dialogUIs> dUIRef;

    ConfigCardRecyclerViewAdapter(fileTransferClient downloader, dialogUIs dui){
        downloadRef = new WeakReference<>(downloader);
        dUIRef = new WeakReference<>(dui);
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
        configResponse.configInfo info = downloadRef.get().getAvailableConfigFiles().get(i);
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
        return downloadRef.get().getAvailableConfigFiles().size();
    }

}


package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.volumeResponse;

public class dialogUIs {
    public static Activity activity;
    final static String TAG = "dialogUIs";
    private static fileTransferClient downloader;
    private TextView errText;
    private Button sendButton;
    private boolean remote_connection_success = false;
    private static AlertDialog loadlocal_dialog, loadremote_dialog, progress_dialog;
    public dialogUIs(final Activity activity_){
        activity = activity_;
    }

    public void SetupConnectRemote(){
        if(remote_connection_success) {
            loadremote_dialog.show();
        }else{
            final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
            final View dialogView = LayoutInflater.from(activity).inflate(R.layout.connect_dialog_layout, null);
            //widgets
            sendButton = (Button) dialogView.findViewById(R.id.connect_req_button);
            EditText hostEdit = (EditText) dialogView.findViewById(R.id.host_edit_text);
            hostEdit.setHint(activity.getString(R.string.DEFAULT_HOST));
            EditText portEdit = (EditText) dialogView.findViewById(R.id.port_edit_text);
            portEdit.setHint(activity.getString(R.string.DEFAULT_PORT));

            errText = (TextView) dialogView.findViewById(R.id.grpc_err_msg);
            errText.setMovementMethod(new ScrollingMovementMethod());

            layoutDialog_builder.setTitle(activity.getString(R.string.dialog_connect_title));
            layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);

            layoutDialog_builder.setView(dialogView);
            AlertDialog connect_dialog = layoutDialog_builder.create();

    //        dialog.setCanceledOnTouchOutside(false);

            sendButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    sendButton.setEnabled(false);
                    errText.setText("");
                    String host_addr = hostEdit.getText().toString();
                    host_addr = host_addr.isEmpty() ? hostEdit.getHint().toString() : host_addr;
                    String port_addr = portEdit.getText().toString();
                    port_addr = port_addr.isEmpty() ? portEdit.getHint().toString() : port_addr;
                    if (downloader == null) downloader = new fileTransferClient(activity);
                    String res_msg = downloader.Setup(host_addr, port_addr);
                    if (res_msg.equals("")) {
                        Log.i(TAG, "=====Connect to server successfully=====");
                        connect_dialog.dismiss();
                        remote_connection_success = true;
                        if (loadremote_dialog == null) SetupDownloadDialog(false);
                        loadremote_dialog.show();
                    } else {
                        errText.setText(res_msg);
                        errText.setVisibility(View.VISIBLE);
                        sendButton.setEnabled(true);
                    }
                }
            });
            connect_dialog.show();
        }
    }
    public void SetupConnectLocal(){
        if(downloader == null)downloader = new fileTransferClient(activity);
        downloader.SetupLocal();

        if(loadlocal_dialog == null) SetupDownloadDialog(true);
        loadlocal_dialog.show();
    }
    private void SetupDownloadDialog(boolean local){
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);

        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.download_dialog_layout, null);

        //recycle view
        RecyclerView content_view = dialogView.findViewById(R.id.contentRecView);
//        content_view.setHasFixedSize(true);
        //layout manager
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        content_view.setLayoutManager(layout_manager);
        //adapter
        content_view.setAdapter(new DSCardRecyclerViewAdapter(activity, content_view, downloader, local));

        layoutDialog_builder.setTitle(activity.getString(R.string.dialog_select_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);
        if(local) loadlocal_dialog = layoutDialog_builder.create();
        else loadremote_dialog = layoutDialog_builder.create();
//        download_dialog.setCanceledOnTouchOutside(false);
    }
    private static void SetupProgressDialog(String info){
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.progress_dialog, null);
        layoutDialog_builder.setTitle(activity.getString(R.string.dialog_progress_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);

        TextView tv = dialogView.findViewById(R.id.textProgressInfo);
        tv.setText(info);

        progress_dialog = layoutDialog_builder.create();
        progress_dialog.setCanceledOnTouchOutside(false);
        progress_dialog.show();
    }
    static void RequestVolumeFromDataset(String dataset_name, int pos, boolean isLocal){
        volumeResponse.volumeInfo vol_info = downloader.getAvailableVolumes(dataset_name, isLocal).get(pos);
        JNIInterface.JNIsetupDCMIConfig(vol_info.getImgWidth(), vol_info.getImgHeight(), vol_info.getFileNums(), vol_info.getMaskAvailable());

        downloader.Download(dataset_name, vol_info);

        //downloading...
        activity.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                if(isLocal)loadlocal_dialog.dismiss();
                else loadremote_dialog.dismiss();
                SetupProgressDialog(dataset_name);
            }});
    }

    public static void FinishMaskLoading(){
        activity.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                Toast.makeText(activity, "Masks Loaded!", Toast.LENGTH_LONG).show();

            }});
    }
    public void updateOnFrame(){
        if(downloader == null) return;
        if(downloader.isDownloadingProcessFinished()){
            downloader.Reset();
            JNIInterface.JNIAssembleVolume();
            activity.runOnUiThread(new Runnable()  {
                @Override
                public void run()  {
                    if(progress_dialog!=null) progress_dialog.dismiss();
                }});
        }
        if(downloader.isDownloadingMaskProcessFinished()){
            downloader.ResetMast();
            JNIInterface.JNIAssembleVolume();
        }
    }
}

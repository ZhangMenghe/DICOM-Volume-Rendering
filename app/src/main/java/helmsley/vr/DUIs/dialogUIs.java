package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.method.ScrollingMovementMethod;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.proto.fileTransferClient;

public class dialogUIs {
    private static WeakReference<Activity> activityReference;
    private static WeakReference<mainUIs> muiRef;

    final static String TAG = "dialogUIs";
    private static fileTransferClient downloader;
    private TextView errText;
    private Button sendButton;
    private boolean remote_connection_success = false;
    private static AlertDialog loadlocal_dialog=null, loadremote_dialog=null,
            loadconfig_dialog=null, saveconfig_dialog=null,
            progress_dialog=null;
    private static DSCardRecyclerViewAdapter loadlocal_adapter;
    public static boolean local_dirty = true;
    private final WeakReference<ViewGroup> parentRef;
    private final int DIALOG_HEIGHT_LIMIT, DIALOG_WIDTH_LIMIT;
    private boolean b_await_data = false, b_await_config=false, b_await_config_export=false;
    enum DownloadDialogType{CONFIGS, DATA_LOCAL, DATA_REMOTE}
    dialogUIs(final Activity activity_, mainUIs mui, ViewGroup parent_view){
        activityReference = new WeakReference<>(activity_);
        muiRef = new WeakReference<>(mui);
        parentRef = new WeakReference<>(parent_view);
        DisplayMetrics displayMetrics = new DisplayMetrics();
        activity_.getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        DIALOG_HEIGHT_LIMIT = (int)(displayMetrics.heightPixels * 0.85);
        DIALOG_WIDTH_LIMIT = (int)(displayMetrics.widthPixels * 0.9);
    }
    void ShowDatasetRemote(){
        if(!remote_connection_success){b_await_data=true;setup_remote_connection();}
        else{
            boolean setup = false;
            if (loadremote_dialog == null) {loadremote_dialog = setup_download_dialog(DownloadDialogType.DATA_REMOTE);setup=true;}
            else loadremote_dialog.invalidateOptionsMenu();
            //order matters
            loadremote_dialog.show();
            if(setup)loadremote_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
        }
    }
    void ShowConfigsRemote(){
        if(!remote_connection_success){b_await_config=true;setup_remote_connection();}
        else{
            boolean setup = false;
            if (loadconfig_dialog == null) {loadconfig_dialog = setup_download_dialog(DownloadDialogType.CONFIGS);setup=true;}
            else loadconfig_dialog.invalidateOptionsMenu();
            //order matters
            loadconfig_dialog.show();
            if(setup)loadconfig_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
        }
    }
    void ExportConfigs(){
        if(!remote_connection_success){b_await_config_export=true;setup_remote_connection();}
        else{
            if (saveconfig_dialog == null) setup_export_dialog();
            else saveconfig_dialog.invalidateOptionsMenu();
            //order matters
            saveconfig_dialog.show();
//            downloader.ExportConfig(content);
        }
    }
    void LoadConfig(String content){
        muiRef.get().LoadConfig(content);
        loadconfig_dialog.dismiss();
    }
    private void setup_export_dialog(){
        Activity activity = activityReference.get();
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.export_dialog_layout, parentRef.get(), false);
        //widgets
        Button btn = (Button) dialogView.findViewById(R.id.req_button);


        layoutDialog_builder.setTitle(activity.getString(R.string.dialog_exp_config_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);

        layoutDialog_builder.setView(dialogView);
        saveconfig_dialog = layoutDialog_builder.create();
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendButton.setEnabled(false);
                EditText nameEdit = (EditText) dialogView.findViewById(R.id.expname_edit_text);
                EditText commnetEdit = (EditText) dialogView.findViewById(R.id.expcomment_edit_text);
                String input_name = nameEdit.getText().toString();
                downloader.ExportConfig(muiRef.get().getExportConfig(input_name.isEmpty()?nameEdit.getHint().toString():input_name,
                        commnetEdit.getText().toString() ));
                saveconfig_dialog.dismiss();
            }
        });
        saveconfig_dialog.show();
    }
    private void setup_remote_connection(){
        Activity activity = activityReference.get();
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.connect_dialog_layout, parentRef.get(), false);
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
                    if (b_await_data){
                        loadremote_dialog = setup_download_dialog(DownloadDialogType.DATA_REMOTE);
                        //order matters
                        loadremote_dialog.show();loadremote_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                        b_await_data = false;
                    }else if(b_await_config){
                        loadconfig_dialog = setup_download_dialog(DownloadDialogType.CONFIGS);
                        loadconfig_dialog.show();loadconfig_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                        b_await_config = false;
                    }else if(b_await_config_export){
                        setup_export_dialog();
                        saveconfig_dialog.show();
                        b_await_config_export = false;
                    }
                } else {
                    errText.setText(res_msg);
                    errText.setVisibility(View.VISIBLE);
                    sendButton.setEnabled(true);
                    connect_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                }
            }
        });
        connect_dialog.show();
    }
    void SetupConnectRemote(){
        if(remote_connection_success) {
            loadremote_dialog.invalidateOptionsMenu();
            loadremote_dialog.show();
        }else{
            Activity activity = activityReference.get();
            final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
            final View dialogView = LayoutInflater.from(activity).inflate(R.layout.connect_dialog_layout, parentRef.get(), false);
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
//                        if (loadremote_dialog == null)
//                            SetupDownloadDialog(false);
//                        //order matters
//                        loadremote_dialog.show();
//                        loadremote_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                    } else {
                        errText.setText(res_msg);
                        errText.setVisibility(View.VISIBLE);
                        sendButton.setEnabled(true);
                        connect_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                    }
                }
            });
            connect_dialog.show();
        }
    }
    void SetupConnectLocal(){
        if(downloader == null)downloader = new fileTransferClient(activityReference.get());
        downloader.SetupLocal();

        if(loadlocal_dialog == null) {loadlocal_dialog = setup_download_dialog(DownloadDialogType.DATA_LOCAL); local_dirty = false;}
        if(local_dirty){ NotifyChanges(); local_dirty=false;}
        loadlocal_dialog.show();
        loadlocal_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
    }

    static void NotifyChanges(){loadlocal_adapter.onContentChange();loadlocal_adapter.notifyDataSetChanged();}

    private AlertDialog setup_download_dialog(DownloadDialogType type){
        final Activity activity = activityReference.get();
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.download_dialog_layout, parentRef.get(), false);
        //recycle view
        RecyclerView content_view = dialogView.findViewById(R.id.contentRecView);
        //layout manager
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        content_view.setLayoutManager(layout_manager);
        //adapter
        switch (type){
            case CONFIGS:
                content_view.setAdapter(new DSCardRecyclerViewAdapter(activity, content_view,downloader, this, DownloadDialogType.CONFIGS));
                break;
            case DATA_LOCAL:
                loadlocal_adapter = new DSCardRecyclerViewAdapter(activity, content_view, downloader, this, DownloadDialogType.DATA_LOCAL);
                content_view.setAdapter(loadlocal_adapter);
                break;
            case DATA_REMOTE:
                content_view.setAdapter(new DSCardRecyclerViewAdapter(activity, content_view, downloader, this, DownloadDialogType.DATA_REMOTE));
                break;
            default:
                return null;
        }
        layoutDialog_builder.setTitle(activity.getString((type == DownloadDialogType.CONFIGS)?R.string.dialog_config_title:R.string.dialog_select_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);
        return layoutDialog_builder.create();
    }

    private static void SetupProgressDialog(String info){
        Activity activity = activityReference.get();
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);
        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.progress_dialog, null);
        layoutDialog_builder.setTitle(activity.getString(R.string.dialog_progress_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);

        //todo: if the info is needed, use adapter
//        TextView tv = dialogView.findViewById(R.id.textProgressInfo);
//        tv.setText(info);

        progress_dialog = layoutDialog_builder.create();
        progress_dialog.setCanceledOnTouchOutside(false);
    }
    static void onDownloadingUI(String dataset_name, boolean isLocal){
        activityReference.get().runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                if(isLocal)loadlocal_dialog.dismiss();
                else loadremote_dialog.dismiss();
                if(progress_dialog == null) SetupProgressDialog(dataset_name);
                progress_dialog.show();
            }});
    }

    public static void FinishMaskLoading(){
        activityReference.get().runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                Toast.makeText( activityReference.get(), "Mask Loaded!", Toast.LENGTH_LONG).show();

            }});
    }
    public void updateOnFrame(){
        if(downloader == null) return;
        if(downloader.isDownloadingProcessFinished()){
            downloader.Reset();
            JNIInterface.JNIsendDataDone();
            activityReference.get().runOnUiThread(new Runnable()  {
                @Override
                public void run()  {
                    if(progress_dialog!=null) progress_dialog.dismiss();
                }});
        }
        if(downloader.isDownloadingMaskProcessFinished()){
            downloader.ResetMast();
            JNIInterface.JNIsendDataDone();
        }
    }

}

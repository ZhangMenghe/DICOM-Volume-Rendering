package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Environment;
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

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.JNIInterface;
import helmsley.vr.MainActivity;
import helmsley.vr.R;
import helmsley.vr.Utils.AVIRecorder;
import helmsley.vr.Utils.AudioRecorder;
import helmsley.vr.Utils.PermissionHelper;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.rpcManager;
import helmsley.vr.proto.volumeInfo;
import helmsley.vr.proto.volumeResponse;

public class dialogUIs {
    private static WeakReference<Activity> activityReference;
    private static WeakReference<mainUIs> muiRef;

    private final static String TAG = "dialogUIs";
    private rpcManager rpc_manager;
    private TextView errText;
    private Button sendButton;
    private boolean remote_connection_success = false;
    private static AlertDialog loadlocal_dialog=null, loadremote_dialog=null,
            loadconfig_dialog=null, saveconfig_dialog=null;
    public static boolean local_dirty = true;
    private final WeakReference<ViewGroup> parentRef;
    private final int DIALOG_HEIGHT_LIMIT, DIALOG_WIDTH_LIMIT;
    private boolean b_await_data = false, b_await_config=false, b_await_config_export=false, b_await_broadcast = false;
    private boolean b_init_pick_alert = false;
    private DSCardRecyclerViewAdapter local_card_adp;
    private ConfigCardRecyclerViewAdapter config_adp;
    private View download_progress, main_progress, broadcast_icon;
    private boolean remote_layout_set = false, config_layout_set = false;

    //recording
    private AVIRecorder mAVIRecorder;
    private AudioRecorder mAudioRecorder;
    private boolean mIsRecording = false;
    private Button recording_button;

    enum DownloadDialogType{CONFIGS, DATA_LOCAL, DATA_REMOTE}
    dialogUIs(final Activity activity_, mainUIs mui, ViewGroup parent_view){
        activityReference = new WeakReference<>(activity_);
        muiRef = new WeakReference<>(mui);
        parentRef = new WeakReference<>(parent_view);
        rpc_manager = new rpcManager(activity_, this);
        DisplayMetrics displayMetrics = new DisplayMetrics();
        activity_.getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        DIALOG_HEIGHT_LIMIT = (int)(displayMetrics.heightPixels * 0.85);
        DIALOG_WIDTH_LIMIT = (int)(displayMetrics.widthPixels * 0.9);

        loadremote_dialog = setup_download_dialog(DownloadDialogType.DATA_REMOTE);
        loadconfig_dialog = setup_download_dialog(DownloadDialogType.CONFIGS);

        loadlocal_dialog = setup_download_dialog(DownloadDialogType.DATA_LOCAL);
        main_progress = activity_.findViewById(R.id.loading_layout);
        broadcast_icon = activity_.findViewById(R.id.broadcast_img);

        setup_recording();
    }
    void ShowDatasetRemote(){
        if(!remote_connection_success){b_await_data=true;setup_remote_connection();}
        else{
            loadremote_dialog.invalidateOptionsMenu();
            //order matters
            loadremote_dialog.show();
            if(!remote_layout_set){loadremote_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);remote_layout_set=true;}
        }
    }
    void ShowConfigsRemote(){
        if(!remote_connection_success){b_await_config=true;setup_remote_connection();}
        else{
            loadconfig_dialog.invalidateOptionsMenu();
            loadconfig_dialog.show();
            if(!config_layout_set){loadconfig_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);remote_layout_set=true;}
        }
    }
    void ExportConfigs(){
        if(!remote_connection_success){b_await_config_export=true;setup_remote_connection();}
        else{
            if (saveconfig_dialog == null) setup_export_dialog();
            else saveconfig_dialog.invalidateOptionsMenu();
            //order matters
            saveconfig_dialog.show();
        }
    }
    void LoadConfig(String content){
        muiRef.get().LoadConfig(content);
        loadconfig_dialog.dismiss();
    }
    void StartBroadcast(){
        if(!remote_connection_success){b_await_broadcast=true;setup_remote_connection();}
        else on_start_broadcast();
    }
    void StopBroadcast(){
        JUIInterface.setBroadcast(false);
        broadcast_icon.setVisibility(View.GONE);
    }
    private void on_start_broadcast(){
        LinkedHashMap map = new LinkedHashMap();
        muiRef.get().mUIManagerRef.get().getCurrentStates(map);
        broadcast_icon.setVisibility(View.VISIBLE);
        JUIInterface.setBroadcast(true);
        JUIInterface.JUIonChangeVolume(rpc_manager.getTargetDatasetName(), rpc_manager.getTargetVolumeName());
        muiRef.get().mUIManagerRef.get().RequestResetWithTemplate(map, false);
    }
    public void NotifyLocalCardUpdate(String ds_name, List<volumeInfo> info_lst){
        local_card_adp.updateLstContent(ds_name, info_lst);
        local_card_adp.updateCardContent();
    }
    public void onChangeRecordingPanel(){
        boolean isOn = (recording_button.getVisibility() == View.VISIBLE);
        recording_button.setVisibility(isOn? View.INVISIBLE:View.VISIBLE);
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
                EditText commentEdit = (EditText) dialogView.findViewById(R.id.expcomment_edit_text);
                String input_name = nameEdit.getText().toString();
                config_adp.ExportConfig(muiRef.get().getExportConfig(input_name.isEmpty()?nameEdit.getHint().toString():input_name,
                        commentEdit.getText().toString() ));
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
                String res_msg = rpc_manager.Setup(host_addr, port_addr);
                if (res_msg.equals("")) {
                    Log.i(TAG, "=====Connect to server successfully=====");
                    connect_dialog.dismiss();
                    config_adp.SetupContents();

                    remote_connection_success = true;
                    if (b_await_data){
                        loadremote_dialog = setup_download_dialog(DownloadDialogType.DATA_REMOTE);
                        //order matters
                        loadremote_dialog.show();loadremote_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                        remote_layout_set=true;
                        b_await_data = false;
                    }else if(b_await_config){
                        loadconfig_dialog = setup_download_dialog(DownloadDialogType.CONFIGS);
                        loadconfig_dialog.show();loadconfig_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
                        b_await_config = false;
                        config_layout_set = true;
                    }else if(b_await_config_export){
                        setup_export_dialog();
                        saveconfig_dialog.show();
                        b_await_config_export = false;
                    }else if(b_await_broadcast){
                        on_start_broadcast();
                        b_await_broadcast = false;
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
    void SetupConnectLocal(){
        rpc_manager.SetupLocal();
        loadlocal_dialog.show();
        loadlocal_dialog.getWindow().setLayout(DIALOG_WIDTH_LIMIT, DIALOG_HEIGHT_LIMIT);
    }
    void showProgress(){
        download_progress.setVisibility(View.VISIBLE);
    }
    void hideProgress(){
        download_progress.setVisibility(View.GONE);
    }

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
                config_adp = new ConfigCardRecyclerViewAdapter(rpc_manager, this);
                content_view.setAdapter(config_adp);
                break;
            case DATA_LOCAL:
                local_card_adp = new DSCardRecyclerViewAdapter(activity, content_view, rpc_manager.getDataManager(), this, DownloadDialogType.DATA_LOCAL);
                content_view.setAdapter(local_card_adp);
                break;
            case DATA_REMOTE:
                download_progress = dialogView.findViewById(R.id.loading_layout);
                DSCardRecyclerViewAdapter remote_card_adp = new DSCardRecyclerViewAdapter(activity, content_view, rpc_manager.getDataManager(), this, DownloadDialogType.DATA_REMOTE);
                content_view.setAdapter(remote_card_adp);
                break;
            default:
                return null;
        }
        final TextView tv = dialogView.findViewById(R.id.title_name);
        tv.setText((type == DownloadDialogType.CONFIGS)?R.string.dialog_config_title:R.string.dialog_select_title);
        layoutDialog_builder.setView(dialogView);
        return layoutDialog_builder.create();
    }

    private void setup_recording(){
        mAVIRecorder = new AVIRecorder();
        mAudioRecorder = new AudioRecorder();

        recording_button = activityReference.get().findViewById(R.id.record_button);
        recording_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mIsRecording) {
                    mAVIRecorder.onStopRecordingNS();
                    mAudioRecorder.onStopRecording();
                    ((Button)v).setText("Record");
                } else {
                    String filename = new Date().getTime() + "";
                    File file_audio = new File(Environment.getExternalStoragePublicDirectory(
                            Environment.DIRECTORY_MOVIES), filename+".3gp");
                    mAudioRecorder.onStartRecording(file_audio.getAbsolutePath());

                    File file = new File(Environment.getExternalStoragePublicDirectory(
                            Environment.DIRECTORY_MOVIES), filename+".avi");
                    mAVIRecorder.onStartRecordingNS(file.getAbsolutePath());

                    ((Button)v).setText("Stop");
                }
                mIsRecording=!mIsRecording;
            }
        });
        recording_button.setVisibility(View.INVISIBLE);
    }

    void onDownloadingUI(boolean isLocal){
        activityReference.get().runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                if(isLocal)loadlocal_dialog.dismiss();
                else loadremote_dialog.dismiss();
                main_progress.setVisibility(View.VISIBLE);
            }});
    }

    public static void FinishMaskLoading(){
        activityReference.get().runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                Toast.makeText( activityReference.get(), "Mask Loaded!", Toast.LENGTH_LONG).show();

            }});
    }
    void updateOnFrame(){
        if(rpc_manager == null) return;
        if(rpc_manager.CheckProcessFinished())onProgressFinish();
    }
    public void onProgressFinish(){
        activityReference.get().runOnUiThread(new Runnable()  {
            @Override
            public void run(){
                main_progress.setVisibility(View.GONE);
            }});
    }
    void ShowDICOMPicker(){
        if(!b_init_pick_alert){
            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(activityReference.get());
            alertDialogBuilder.setMessage("Choose a DICOM file. To view a folder, choose ONE file inside");
            alertDialogBuilder.setPositiveButton("yes",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface arg0, int arg1) {
                            show_file_picker();
                        }
                    });

            AlertDialog alertDialog = alertDialogBuilder.create();
            alertDialog.show();
            b_init_pick_alert = true;
        }else{
            show_file_picker();
        }
    }
    private void show_file_picker(){
        if(!MainActivity.permission_granted)
            Toast.makeText(
                    activityReference.get(),
                    "Permission denied to read your External storage, try to enable it via system setting",
                    Toast.LENGTH_SHORT
            ).show();
        // Let's use the Android File dialog. It will return an answer in the future, which we
        // get via onActivityResult()
        Intent intent = new Intent()
                .setType("*/*")
                .setAction(Intent.ACTION_OPEN_DOCUMENT);

        activityReference.get().
                startActivityForResult(
                        Intent.createChooser(intent, "Select a DICOM file"),
                        PermissionHelper.FILE_PERMISSION_CODE
                );

    }
}

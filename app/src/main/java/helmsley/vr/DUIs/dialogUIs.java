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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.proto.fileTransferClient;

public class dialogUIs {
    public static Activity activity;
    final static String TAG = "dialogUIs";
    private static fileTransferClient downloader;
    private TextView errText;
    private Button sendButton;

    private static AlertDialog download_dialog, progress_dialog;
//    private ArrayList<datasetInfo> data_info_lst;
    public dialogUIs(final Activity activity_){
        activity = activity_;
        if(LoadCachedData("dicom_sample")) return;
        SetupConnect();
    }

    public boolean LoadCachedData(String folder_name){
        if(!Boolean.parseBoolean(activity.getString(R.string.cf_b_loadcache))) return false;
        String target_cached_folder_path = activity.getFilesDir().getAbsolutePath() + "/" + activity.getString(R.string.cf_cache_folder_name);
        //check exists
        File destDir = new File(target_cached_folder_path);
        if(!destDir.exists()) return false;
        ArrayList<String> config_lines = new ArrayList<>();
        //config
        try{
            InputStream inputStream = new FileInputStream(Paths.get(target_cached_folder_path, folder_name, activity.getString(R.string.cf_config_name)).toString());//target_cached_folder_path + "/sample/config");

            if ( inputStream != null ) {
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                String receiveString = "";

                while ( (receiveString = bufferedReader.readLine()) != null ) config_lines.add(receiveString);
                inputStream.close();
            }
        }catch (FileNotFoundException e) {
            Log.e(TAG, "===File not found: " + e.toString());
            return false;
        } catch (IOException e) {
            Log.e(TAG, "===Can not read file: " + e.toString());
            return false;
        }
        if(config_lines.size() < 4) return false;
        JNIInterface.JNIsetupDCMIConfig(Integer.parseInt(config_lines.get(2)), Integer.parseInt(config_lines.get(3)), Integer.parseInt(config_lines.get(1)));


        Path dcm_img = Paths.get(target_cached_folder_path, folder_name, activity.getString(R.string.cf_dcmfolder_name));
        try{
            byte[] content = Files.readAllBytes(dcm_img);
            JNIInterface.JNIsendDCMImg(-1, .0f, content);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }
    public void SetupConnect(){
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
        AlertDialog dialog = layoutDialog_builder.create();

        dialog.setCanceledOnTouchOutside(false);

        sendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                ((InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE))
//                        .hideSoftInputFromWindow(hostEdit.getWindowToken(), 0);
                sendButton.setEnabled(false);
                errText.setText("");
                String host_addr = hostEdit.getText().toString();
                host_addr = host_addr.isEmpty()?hostEdit.getHint().toString():host_addr;
                String port_addr = portEdit.getText().toString();
                port_addr = port_addr.isEmpty()?portEdit.getHint().toString():port_addr;

                if(SetupDownloader(host_addr, port_addr)){
                    Log.i(TAG, "====Connect to server successfully=====");
                    dialog.dismiss();
                    SetupDownloadDialog();
                }
            }
        });

        dialog.show();
    }
    private void SetupDownloadDialog(){
        final AlertDialog.Builder layoutDialog_builder = new AlertDialog.Builder(activity);

        final View dialogView = LayoutInflater.from(activity).inflate(R.layout.download_dialog_layout, null);

        //recycle view

        RecyclerView content_view = dialogView.findViewById(R.id.contentRecView);
//        content_view.setHasFixedSize(true);
        //layout manager
        RecyclerView.LayoutManager layout_manager = new LinearLayoutManager(activity);
        content_view.setLayoutManager(layout_manager);
        //adapter
        content_view.setAdapter(new DialogAdapter(activity, content_view, downloader.getAvailableDataset()));

        layoutDialog_builder.setTitle(activity.getString(R.string.dialog_select_title));
        layoutDialog_builder.setIcon(R.mipmap.ic_launcher_round);
        layoutDialog_builder.setView(dialogView);
        download_dialog = layoutDialog_builder.create();
        download_dialog.setCanceledOnTouchOutside(false);

        download_dialog.show();

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

    private boolean SetupDownloader(String host, String port){
        downloader = new fileTransferClient(activity);//new fileTransferClient(host, port);

        String res_msg = downloader.Setup(host, port);
        if(res_msg.equals(""))
            return true;

        errText.setText(res_msg);
        errText.setVisibility(View.VISIBLE);
        sendButton.setEnabled(true);
        return false;
    }
    public static void Download(String folder_name){
        downloader.Download(folder_name);
        download_dialog.dismiss();
        SetupProgressDialog(folder_name);
    }
    public static void FinishProgress(){progress_dialog.dismiss();}
}

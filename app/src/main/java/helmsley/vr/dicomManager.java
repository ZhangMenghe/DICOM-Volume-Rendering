package helmsley.vr;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.protobuf.ByteString;
import com.imebra.CodecFactory;
import com.imebra.ColorTransformsFactory;
import com.imebra.DataSet;
import com.imebra.DrawBitmap;
import com.imebra.Image;
import com.imebra.Memory;
import com.imebra.PipeStream;
import com.imebra.StreamReader;
import com.imebra.TagId;
import com.imebra.TransformsChain;
import com.imebra.VOILUT;
import com.imebra.drawBitmapType_t;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Random;

import helmsley.vr.DUIs.DSCardRecyclerViewAdapter;
import helmsley.vr.Utils.PushToImebraPipe;
import helmsley.vr.proto.datasetResponse;
import helmsley.vr.proto.fileTransferClient;
import helmsley.vr.proto.volumeResponse;

import static helmsley.vr.DUIs.dialogUIs.progress_dialog;

public class dicomManager {
    private static final String TAG = "dicomManager";
    private final WeakReference<Activity> actRef;
    private AlertDialog file_dir_dialog = null, preview_dialog = null;
    private Uri sel_uri;
    private static boolean is_finished = false;
    private ImageView preview_img_view;
    private TextView title_tex_view, content_tex_view;
    private static Image single_image;
    public static String DEFAULT_DS_NAME;
    dicomManager(Activity activity) {
        actRef = new WeakReference<>(activity);
        DEFAULT_DS_NAME = activity.getString(R.string.data_device_dir_name);
    }

    private void setup_volume_data(boolean isVolume) {
        String rpath = sel_uri.getPathSegments().get(1);
        String[] splits = rpath.split(":");
        if (splits.length <= 1 || !isVolume) {
            InputStream mstream;
            try {
                mstream = actRef.get().getContentResolver().openInputStream(sel_uri);
            } catch (IOException e) {
                AlertDialog.Builder dlgAlert = new AlertDialog.Builder(actRef.get());
                dlgAlert.setMessage(e.getMessage());
                dlgAlert.setTitle("Error");
                dlgAlert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        //dismiss the dialog
                    }
                });
                dlgAlert.setCancelable(true);
                dlgAlert.create().show();
                return;
            }
            String[] name_split = rpath.split("/");
            build_single_slice_strem(mstream, name_split[name_split.length - 1]);
            if (isVolume)
                Toast.makeText(actRef.get(), "remote side only supports single file", Toast.LENGTH_LONG);
            return;
        }
        List<String> streams = new ArrayList<>();
        File sel_file = new File(splits[1]);
        //build stream list
        File pd = sel_file.getParentFile();
        String[] flst = pd.list();
        for (String f : flst) streams.add(pd.getAbsolutePath()+'/'+f);
        build_volume(pd.getAbsolutePath(), pd.getName(), streams);
    }

    private void show_file_dir_dialog() {
        if (file_dir_dialog == null) {
            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(actRef.get());
            alertDialogBuilder.setMessage("Do you want to see the 3D volume rather than the image?");
            alertDialogBuilder.setPositiveButton("YES",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface arg0, int arg1) {
                            setup_volume_data(true);
                        }
                    });
            alertDialogBuilder.setNegativeButton("NO", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    setup_volume_data(false);
                }
            });
            file_dir_dialog = alertDialogBuilder.create();
        }
        file_dir_dialog.show();
    }

    void Run(Uri selectedfile) {
        sel_uri = selectedfile;
        show_file_dir_dialog();
    }

    private void build_single_slice_strem(InputStream stream, String name) {
        CodecFactory.setMaximumImageSize(8000, 8000);

        // The usage of the Pipe allows to use also files on Google Drive or other providers
        PipeStream imebraPipe = new PipeStream(32000);

        // Launch a separate thread that read from the InputStream and pushes the data
        // to the Pipe.
        Thread pushThread = new Thread(new PushToImebraPipe(imebraPipe, stream));
        pushThread.start();

        // The CodecFactory will read from the Pipe which is feed by the thread launched
        // before. We could just pass a file name to it but this would limit what we
        // can read to only local files

        DataSet loadDataSet = CodecFactory.load(new StreamReader(imebraPipe.getStreamInput()));
        // Get the first frame from the dataset (after the proper modality transforms
        // have been applied).
        single_image = loadDataSet.getImageApplyModalityTransform(0);
        byte[] byte_data = get_image_byte_array(single_image);

        // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
        Bitmap renderBitmap = Bitmap.createBitmap((int) single_image.getWidth(), (int) single_image.getHeight(), Bitmap.Config.ARGB_8888);

        ByteBuffer byteBuffer = ByteBuffer.wrap(byte_data);
        renderBitmap.copyPixelsFromBuffer(byteBuffer);

        //update image
        if(preview_dialog == null) setup_dialog();
        preview_img_view.setImageBitmap(renderBitmap);
        title_tex_view.setText(actRef.get().getString(R.string.preview_text,name));
        //name
        String content = "Patient Name: " + loadDataSet.getString(new TagId(0x0010, 0x0010), 0) + '\n';
        content += "Date: " + loadDataSet.getString(new TagId(0x0008, 0x0023), 0) ;
        content_tex_view.setText(content);
        preview_dialog.show();
    }
    private void setup_dialog(){
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
        Button delete_btn = dialogView.findViewById(R.id.pre_delete_btn);
        delete_btn.setVisibility(View.GONE);
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
                long width, height; int ssize;
                width = single_image.getWidth(); height = single_image.getHeight();
                ssize = (int)(width * height) * 2;
                byte[] data = get_image_pure_bytes(single_image, ssize, width, height);
                JNIInterface.JNIsendDataPrepare((int)width, (int)height, 1, -1, false);
                JNIInterface.JNIsendData(0, 0, ssize, 2, data);
                is_finished = true;
                preview_dialog.dismiss();
            }
        });
    }
    private void build_volume(String folder_path, String vol_name, List<String> file_names) {
        long width=0, height=0; int ssize=0;
        byte[][] datas = new byte[file_names.size()][];
        boolean is_first = true;
        DataSet loadDataSet;
        datasetResponse.datasetInfo.Builder ds_builder = datasetResponse.datasetInfo.newBuilder();
        volumeResponse.volumeInfo.Builder vinfo_builder = volumeResponse.volumeInfo.newBuilder()
                .setFolderName(vol_name)
                .setFolderPath(folder_path)
                .setVolumeLocRange(-1)
                .setWithMask(false)
                .setDataSource(volumeResponse.volumeInfo.DataSource.DEVICE);

        for(String name:file_names){
            loadDataSet = CodecFactory.load(name);
            Image dicomImage = loadDataSet.getImageApplyModalityTransform(0);
            if(is_first){
                //build dataset info

                width = dicomImage.getWidth(); height = dicomImage.getHeight();
                JNIInterface.JNIsendDataPrepare((int)width, (int)height, file_names.size(), -1, false);
                ssize = (int)(width * height) * 2;

                is_first = false;

//                String patient_name = loadDataSet.getString(new TagId(0x0010, 0x0010), 0);
//                String date_str = loadDataSet.getString(new TagId(0x0008, 0x0023), 0);
//                String physicanName = loadDataSet.getString(new TagId(0x0008,0x0090), 0);
                ds_builder = datasetResponse.datasetInfo.newBuilder().setFolderName(DEFAULT_DS_NAME).setPatientName("UNKNOWN")
                        .setDate("UNKNOWN").setPhysicanName("UNKNOWN");
                TagId po_tag = new TagId(0x0020, 0x0020);
                float o1 = loadDataSet.getFloat(po_tag,0,-100);
                if(o1  != -100){
                    vinfo_builder.addOrientation(o1);
                    for(int i=1; i<6; i++) vinfo_builder.addOrientation(loadDataSet.getFloat(po_tag, i));
                }else{
                    for(int i=0; i<6; i++) vinfo_builder.addOrientation(-1);
                }
                vinfo_builder.addDims((int)height);vinfo_builder.addDims((int)width);vinfo_builder.addDims((int)file_names.size());
                vinfo_builder.addResolution(-1);vinfo_builder.addResolution(-1);
//                Random random = new Random();
                volumeResponse.scoreInfo.Builder s_builder = volumeResponse.scoreInfo.newBuilder()
                        .setRgroupId(-1)
                        .setRankId(-1)
                        .setRankScore(-1);//random.nextFloat());
                for(int ri=3; ri<21; ri++)
                    s_builder.addRawScore(-1);//random.nextFloat());
                for(int ni=21; ni<24; ni++)
                    s_builder.addVolScore(-1);
                vinfo_builder.setScores(s_builder.build());
            }
            int ins_id = Integer.parseInt(loadDataSet.getString(new TagId(0x0020,0x0013),0)) - 1;
            datas[ins_id] = get_image_pure_bytes(dicomImage, ssize, width, height);
        }
        for(int i=0; i<datas.length;i++)
            JNIInterface.JNIsendData(0, i, ssize, 2, datas[i]);
        is_finished = true;
        int sam_size = ssize / 2;
        byte[] sample_data = datas[(int)(datas.length / 2)];
        byte[] simg_bytes = new byte[sam_size];
        for(int i=0; i<sam_size; i++)
            simg_bytes[i] = sample_data[2*i];
        vinfo_builder.setSampleImg(ByteString.copyFrom(simg_bytes));

        fileTransferClient.saveDCMI(ds_builder.build(), vinfo_builder.build(), false);
    }
    private byte[] get_image_byte_array(Image dicomImage){
        TransformsChain chain = new TransformsChain();

        if (ColorTransformsFactory.isMonochrome(dicomImage.getColorSpace())) {
            VOILUT voilut = new VOILUT(VOILUT.getOptimalVOI(dicomImage, 0, 0, dicomImage.getWidth(), dicomImage.getHeight()));
            chain.addTransform(voilut);
        }
        DrawBitmap drawBitmap = new DrawBitmap(chain);
        Memory memory = drawBitmap.getBitmap(dicomImage, drawBitmapType_t.drawBitmapRGBA, 4);

        // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
        byte[] memoryByte = new byte[(int) memory.size()];
        memory.data(memoryByte);
        return  memoryByte;
    }
    private static byte[] get_image_pure_bytes(Image dicomImage, int bsize, long width, long height){
        byte[] data = new byte[bsize];
        // Retrieve the data handler
        com.imebra.ReadingDataHandlerNumeric dataHandler = dicomImage.getReadingDataHandler();

        for(long scanY = 0, idy=0; scanY != height; scanY++, idy+=width){
            for(long scanX = 0; scanX != width; scanX++){
                // For monochrome images
                long idx = idy + scanX;
                int value = dataHandler.getSignedLong( idx);
                data[2*(int)idx+1] = (byte)(value>>>8);
                data[2*(int)idx] = (byte)value;
            }
        }
        return data;
    }
    public static boolean LoadDataFromDevice(volumeResponse.volumeInfo vinfo){
        int height = vinfo.getDims(0);int width = vinfo.getDims(1);int nums = vinfo.getDims(2);
        int ssize = width * height * 2;
        byte[][] datas = new byte[nums][];
        boolean is_first = true;
        DataSet loadDataSet;
        try{
            File pd = new File(vinfo.getFolderPath());

            String[] flst = pd.list();
            for (String f : flst) {
                try {
                    String fname = pd.getAbsolutePath()+'/'+f;
                    loadDataSet = CodecFactory.load(fname);
                    Image dicomImage = loadDataSet.getImageApplyModalityTransform(0);
                    if(is_first) {
                        is_first = false;
                        JNIInterface.JNIsendDataPrepare(width, height, nums, -1, false);
                    }
                    int ins_id = Integer.parseInt(loadDataSet.getString(new TagId(0x0020,0x0013),0)) - 1;
                    datas[ins_id] = get_image_pure_bytes(dicomImage, ssize, width, height);
                } catch (Exception e) {
                    Log.e(TAG, "=====setup_volume_data: ");
                    return false;
                }
            }
        }catch (Exception e){
            return false;
        }
        for(int i=0; i<datas.length;i++)
            JNIInterface.JNIsendData(0, i, ssize, 2, datas[i]);
        is_finished = true;
        return true;

    }
    void updateOnFrame(){
        if(is_finished){
            is_finished = false;
            JNIInterface.JNIsendDataDone();
            actRef.get().runOnUiThread(new Runnable()  {
                @Override
                public void run()  {
                    if(progress_dialog!=null) progress_dialog.dismiss();
                }});
        }
    }
}

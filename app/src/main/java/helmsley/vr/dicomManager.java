package helmsley.vr;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

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
import java.util.List;
import helmsley.vr.Utils.PushToImebraPipe;

class dicomManager {
    private static final String TAG = "dicomManager";
    private final WeakReference<Activity> actRef;
    private AlertDialog file_dir_dialog = null;
    private Uri sel_uri;
    private static boolean is_finished = false;

    dicomManager(Activity activity) {
        actRef = new WeakReference<>(activity);
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
            build_single_slice_strem(mstream);
            if (isVolume)
                Toast.makeText(actRef.get(), "remote side only supports single file", Toast.LENGTH_LONG);
            return;
        }
        List<String> streams = new ArrayList<>();
        File sel_file = new File(splits[1]);
        //build stream list
        File pd = sel_file.getParentFile();
        String[] flst = pd.list();
        for (String f : flst) {
            try {
                streams.add(pd.getAbsolutePath()+'/'+f);
            } catch (Exception e) {
                Log.e(TAG, "=====setup_volume_data: ");
            }
        }
        build_volume(streams);
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

    private void build_single_slice_strem(InputStream stream) {
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
        Image dicomImage = loadDataSet.getImageApplyModalityTransform(0);
        byte[] byte_data = get_image_byte_array(dicomImage);

        // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
        Bitmap renderBitmap = Bitmap.createBitmap((int) dicomImage.getWidth(), (int) dicomImage.getHeight(), Bitmap.Config.ARGB_8888);

        ByteBuffer byteBuffer = ByteBuffer.wrap(byte_data);
        renderBitmap.copyPixelsFromBuffer(byteBuffer);

        // Update the image
//        mImageView.setImageBitmap(renderBitmap);
//        mImageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
    }

    private void build_volume(List<String> file_names) {
        int id = 0;
        long width=0, height=0; int ssize=0;
        byte[][] datas = new byte[file_names.size()][];

        DataSet loadDataSet;
        for(String name:file_names){
            loadDataSet = CodecFactory.load(name);
            Image dicomImage = loadDataSet.getImageApplyModalityTransform(0);
            if(id == 0){
                width = dicomImage.getWidth(); height = dicomImage.getHeight();
                JNIInterface.JNIsendDataPrepare((int)width, (int)height, file_names.size(), -1, false);
                ssize = (int)(width * height) * 2;
            }
            int ins_id = Integer.parseInt(loadDataSet.getString(new TagId(0x0020,0x0013),0)) - 1;
            datas[ins_id] = get_image_pure_bytes(dicomImage, ssize, width, height);
        }
        for(int i=0; i<datas.length;i++)
            JNIInterface.JNIsendData(0, i, ssize, 2, datas[i]);
        is_finished = true;
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
    private byte[] get_image_pure_bytes(Image dicomImage, int bsize, long width, long height){
        byte[] data = new byte[bsize];
        // Retrieve the data handler
        com.imebra.ReadingDataHandlerNumeric dataHandler = dicomImage.getReadingDataHandler();

        for(long scanY = 0, idy=0; scanY != height; scanY++, idy+=width){
            for(long scanX = 0; scanX != width; scanX++){
                // For monochrome images
                long idx = idy + scanX;
                int value = dataHandler.getSignedLong( idx);
                data[2*(int)idx+1] = (byte)(0);
                data[2*(int)idx] = (byte)value;
            }
        }
        return data;
    }
    void updateOnFrame(){
        if(is_finished){
            is_finished = false;
            JNIInterface.JNIsendDataDone();
        }
    }
}

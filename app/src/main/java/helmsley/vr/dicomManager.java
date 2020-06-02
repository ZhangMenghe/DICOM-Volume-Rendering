package helmsley.vr;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.imebra.CodecFactory;
import com.imebra.ColorTransformsFactory;
import com.imebra.DataSet;
import com.imebra.DrawBitmap;
import com.imebra.Image;
import com.imebra.Memory;
import com.imebra.PipeStream;
import com.imebra.StreamReader;
import com.imebra.TransformsChain;
import com.imebra.VOILUT;
import com.imebra.drawBitmapType_t;

import java.io.File;
import java.io.FileInputStream;
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
    private ImageView mImageView;
    private AlertDialog file_dir_dialog = null;
    private Uri sel_uri;
    dicomManager(Activity activity){
        mImageView = activity.findViewById(R.id.debug_img_view);
//        mImageView.setVisibility(View.INVISIBLE);
        actRef = new WeakReference<>(activity);
    }
    private void setup_volume_data(boolean isVolume){
        List<InputStream> streams = new ArrayList<>();
        File sel_file = new File(sel_uri.getPath());
        if(!isVolume) {
            try{
                InputStream stream = actRef.get().getContentResolver().openInputStream(sel_uri);
//                streams.add(stream);
                InputStream stm = new FileInputStream(sel_file);
                streams.add(stm);
            }catch (Exception e){
                Log.e(TAG, "=====setup_volume_data: " );

            }
        }else{
            //build stream list

        }
        build_volume(streams);
    }
    private void show_file_dir_dialog(){
        if(file_dir_dialog == null){
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
    private void build_volume(List<InputStream> streams){
        try {
            CodecFactory.setMaximumImageSize(8000, 8000);
            InputStream stream = actRef.get().getContentResolver().openInputStream(sel_uri);

            // The usage of the Pipe allows to use also files on Google Drive or other providers
            PipeStream imebraPipe = new PipeStream(32000);

            // Launch a separate thread that read from the InputStream and pushes the data
            // to the Pipe.
            Thread pushThread  = new Thread(new PushToImebraPipe(imebraPipe, streams));
            pushThread.start();

            // The CodecFactory will read from the Pipe which is feed by the thread launched
            // before. We could just pass a file name to it but this would limit what we
            // can read to only local files
            DataSet loadDataSet = CodecFactory.load(new StreamReader(imebraPipe.getStreamInput()));
            // Get the first frame from the dataset (after the proper modality transforms
            // have been applied).

            Image dicomImage = loadDataSet.getImageApplyModalityTransform(0);

            // Use a DrawBitmap to build a stream of bytes that can be handled by the
            // Android Bitmap class.
            TransformsChain chain = new TransformsChain();

            if(ColorTransformsFactory.isMonochrome(dicomImage.getColorSpace()))
            {
                VOILUT voilut = new VOILUT(VOILUT.getOptimalVOI(dicomImage, 0, 0, dicomImage.getWidth(), dicomImage.getHeight()));
                chain.addTransform(voilut);
            }
            DrawBitmap drawBitmap = new DrawBitmap(chain);
            Memory memory = drawBitmap.getBitmap(dicomImage, drawBitmapType_t.drawBitmapRGBA, 4);

            // Build the Android Bitmap from the raw bytes returned by DrawBitmap.
            Bitmap renderBitmap = Bitmap.createBitmap((int)dicomImage.getWidth(), (int)dicomImage.getHeight(), Bitmap.Config.ARGB_8888);
            byte[] memoryByte = new byte[(int)memory.size()];
            memory.data(memoryByte);
            ByteBuffer byteBuffer = ByteBuffer.wrap(memoryByte);
            renderBitmap.copyPixelsFromBuffer(byteBuffer);

            // Update the image
            mImageView.setImageBitmap(renderBitmap);
            mImageView.setScaleType(ImageView.ScaleType.FIT_CENTER);

            // Update the text with the patient name
//                mTextView.setText(loadDataSet.getPatientName(new TagId(0x10,0x10), 0, new PatientName("Undefined", "", "")).getAlphabeticRepresentation());
        }
        catch(IOException e) {
            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(actRef.get());
            dlgAlert.setMessage(e.getMessage());
            dlgAlert.setTitle("Error");
            dlgAlert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    //dismiss the dialog
                } } );
            dlgAlert.setCancelable(true);
            dlgAlert.create().show();
            String test = "Test";
        }
    }
}

package helmsley.vr.Utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.util.Log;

import com.imebra.CodecFactory;
import com.imebra.ColorTransformsFactory;
import com.imebra.DataSet;
import com.imebra.DrawBitmap;
import com.imebra.FileStreamInput;
import com.imebra.LUT;
import com.imebra.StreamReader;
import com.imebra.TagId;
import com.imebra.TransformsChain;
import com.imebra.VOILUT;
import com.imebra.VOIs;
import com.imebra.drawBitmapType_t;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class dcmImage{
    static {
        System.loadLibrary("imebra_lib");
    }
    public Bitmap bitmap = null;
    public Bitmap bitmap_msk = null;
    public float location;
    public float xSpacing;
    public float ySpacing;
    public float thickness;

    final static private com.imebra.TagId location_id =
            new com.imebra.TagId(0x0020, 0x1041 );
    final static private com.imebra.TagId Spacing_id =
            new com.imebra.TagId(0x0028, 0x0030 );
    final static private com.imebra.TagId thickness_id =
            new com.imebra.TagId(0x0018, 0x0050 );

    final static private float LOC_PRE_DEFAULT = 1.0f;
    final static String TAG = "DCM_IMAGE_CLASS";
    public dcmImage(String path){
        load_dcm(path, LOC_PRE_DEFAULT);
    }
    public dcmImage(String path, float loc_pre){
        load_dcm(path, loc_pre);
    }
    public dcmImage(String path, String msk_pth){
        Log.i(TAG, "====dcmi:  " + path);
        Log.i(TAG, "====mask:  " + msk_pth);
        load_dcm(path, 1.0f);
        load_mask(msk_pth);
    }

    private void load_dcm(String path, float loc_pre){
        FileStreamInput dcm_stream = new FileStreamInput(path);
        DataSet dcmDataset = CodecFactory.load(new StreamReader(dcm_stream));

        try{
            location = loc_pre * Float.parseFloat(dcmDataset.getString(location_id, 0));
            xSpacing = Float.parseFloat(dcmDataset.getString(Spacing_id, 0));
            ySpacing = Float.parseFloat(dcmDataset.getString(Spacing_id, 1));
            thickness = Float.parseFloat(dcmDataset.getString(thickness_id, 0));
        }catch (Exception e){
            Log.i(TAG, "getDCMIImage: ===mis information===");
            thickness = -1;
            return;
        }

        com.imebra.Image image = dcmDataset.getImageApplyModalityTransform(0);

        // Get the color space
        String colorSpace = image.getColorSpace();

// Get the size in pixels
        long width = image.getWidth();
        long height = image.getHeight();

        TransformsChain chain = new TransformsChain();
        if(ColorTransformsFactory.isMonochrome(image.getColorSpace())) {

            VOILUT voilutTransform = new VOILUT();

            VOIs vois = dcmDataset.getVOIs();

            List<LUT> luts = new ArrayList<LUT>();

            for(long scanLUTs = 0; ; scanLUTs++) {
                try {
                    luts.add(dcmDataset.getLUT(new TagId(0x0028, 0x3010), scanLUTs));
                } catch (Exception e) {
                    break;
                }
            }

            if(!vois.isEmpty()) {
                voilutTransform.setCenterWidth(vois.get(0).getCenter(), vois.get(0).getWidth());
            } else if (!luts.isEmpty()){
                voilutTransform.setLUT(luts.get(0));
            } else {
                voilutTransform.applyOptimalVOI(image, 0, 0, width, height);
            }
            chain.addTransform(voilutTransform);
        }

        DrawBitmap draw = new DrawBitmap(chain);
        long requestedBufferSize = draw.getBitmap(image, drawBitmapType_t.drawBitmapRGBA, 4, new byte[0]);

        byte buffer[] = new byte [(int) requestedBufferSize];
        ByteBuffer byteBuffer = ByteBuffer.wrap(buffer);

        draw.getBitmap(image, drawBitmapType_t.drawBitmapRGBA, 4 , buffer);


        Bitmap tmp_bitmap = Bitmap.createBitmap((int) image.getWidth(), (int) image.getHeight(), Bitmap.Config.ARGB_8888);
        tmp_bitmap.copyPixelsFromBuffer(byteBuffer);

        Matrix matrix = new Matrix();
        matrix.preScale(1.0f, -1.0f);
        bitmap = Bitmap.createBitmap(tmp_bitmap, 0, 0, tmp_bitmap.getWidth(), tmp_bitmap.getHeight(), matrix, true);

//        Bitmap renderBitmap = Bitmap.createBitmap((int) image.getWidth(), (int) image.getHeight(), Bitmap.Config.ARGB_8888);
//        renderBitmap.copyPixelsFromBuffer(byteBuffer);

//        dcm_bitmaps.add(renderBitmap);
    }

    private void load_mask(String path){
        try{
            FileInputStream fis = new FileInputStream(path);
            Bitmap bitmap_msk_tmp = BitmapFactory.decodeStream(fis);

            Matrix matrix = new Matrix();
            matrix.preScale(1.0f, -1.0f);
            bitmap_msk = Bitmap.createBitmap(bitmap_msk_tmp, 0, 0, bitmap_msk_tmp.getWidth(), bitmap_msk_tmp.getHeight(), matrix, true);

        }catch (IOException e){
            Log.e(TAG, "===load_mask error from : " + path);
        }
    }

}
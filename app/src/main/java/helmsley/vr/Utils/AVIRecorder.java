package helmsley.vr.Utils;

import android.util.Log;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.videoio.VideoWriter;

public class AVIRecorder {
    private static final String TAG = "AVIRecorder";

    static VideoWriter videoWriter = null;

    public static void WriteToVideo(byte[] data){
        int w = 460, h=640;
        if(!videoWriter.isOpened()){
            Log.e(TAG, "======debugVideoWriter: fail to open" );
        }else{
            Mat rgbMat = new Mat(h,w, CvType.CV_8UC3);
//                rgbMat.setTo(new Scalar(255,0,0));
            rgbMat.put(0,0, data);
            videoWriter.write(rgbMat);
            rgbMat.release();
        }
    }

    public static void onStartRecording(String filePath){
        int w = 460, h=640;
        double FPS = 30.0;

        if(videoWriter == null){
//            File file = new File(Environment.getExternalStoragePublicDirectory(
//                    Environment.DIRECTORY_MOVIES), "videoWriter.avi");
//            String filePath = file.getAbsolutePath();
            videoWriter = new VideoWriter(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w, h));
        }
        videoWriter.open(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w,h));
    }
    public static void onStopRecording(){
        videoWriter.release();
    }
}

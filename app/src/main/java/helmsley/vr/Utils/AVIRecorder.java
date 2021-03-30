package helmsley.vr.Utils;

import android.util.Log;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.videoio.VideoWriter;

import java.util.Arrays;

public class AVIRecorder {
    private static final String TAG = "AVIRecorder";
    Thread mGrabThread;
    JNIFrameGrabber mGrabRunnable;

    public AVIRecorder(){
        mGrabRunnable = new JNIFrameGrabber();
    }
    public void onStartRecordingNS(String filePath){
        mGrabRunnable.Reset(filePath);
        mGrabThread = new Thread(mGrabRunnable);
        Log.e(TAG, "=====onStartRecordingNS, new thread " );

        mGrabThread.start();
    }
    public void onStopRecordingNS(){
        mGrabRunnable.Stop();
        try {
            Log.e(TAG, "=====onStartRecordingNS, new thread " );
            mGrabThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
            Log.e(TAG, "=====onStopRecordingNS: " );
        } finally {
            mGrabThread = null;
        }
    }
//    public static void WriteToVideo(byte[] data){
//        int w = 460, h=640;
//        if(!videoWriter.isOpened()){
//            Log.e(TAG, "======debugVideoWriter: fail to open" );
//        }else{
//            Mat rgbMat = new Mat(h,w, CvType.CV_8UC3);
////                rgbMat.setTo(new Scalar(255,0,0));
//            rgbMat.put(0,0, data);
//            videoWriter.write(rgbMat);
//            rgbMat.release();
//        }
//    }
//
//    public static void onStartRecording(String filePath){
//        int w = 460, h=640;
//        double FPS = 30.0;
//
//        if(videoWriter == null){
////            File file = new File(Environment.getExternalStoragePublicDirectory(
////                    Environment.DIRECTORY_MOVIES), "videoWriter.avi");
////            String filePath = file.getAbsolutePath();
//            videoWriter = new VideoWriter(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w, h));
//        }
//        videoWriter.open(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w,h));
//    }
//    public static void onStopRecording(){
//        videoWriter.release();
//    }
    private class JNIFrameGrabber implements Runnable{
        private VideoWriter videoWriter = null;

        int mFrames = 0;
        boolean isRunning = true;
        public void Reset(String filePath){
            int w = 460, h=640;
            double FPS = 30.0;

            if(videoWriter == null){
//            File file = new File(Environment.getExternalStoragePublicDirectory(
//                    Environment.DIRECTORY_MOVIES), "videoWriter.avi");
//            String filePath = file.getAbsolutePath();
                videoWriter = new VideoWriter(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w, h));
            }
            videoWriter.open(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, new Size(w,h));

            mFrames = 0;
            isRunning = true;
        }
        @Override
        public void run() {
            while(isRunning){
                int w = 460, h=640;
                if(!videoWriter.isOpened()){
                    Log.e(TAG, "======debugVideoWriter: fail to open" );
                }else{
                    byte[] data = new byte[640 * 480 * 3];
                    Arrays.fill(data, (byte)255);

                    Mat rgbMat = new Mat(h,w, CvType.CV_8UC3);
                    rgbMat.put(0,0, data);
                    videoWriter.write(rgbMat);
                    rgbMat.release();
                }
                mFrames++;
            }
            videoWriter.release();
        }
        public void Stop(){
            isRunning = false;
        }
    }
}

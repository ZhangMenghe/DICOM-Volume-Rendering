package helmsley.vr.Utils;

import android.util.Log;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoWriter;

import helmsley.vr.DUIs.JUIInterface;
import helmsley.vr.JNIInterface;

public class AVIRecorder {
    private static final String TAG = "AVIRecorder";
    Thread mGrabThread;
    static JNIFrameGrabber mGrabRunnable;
    public AVIRecorder(){
        mGrabRunnable = new JNIFrameGrabber();
    }
    public void onStartRecordingNS(String filePath){
        JUIInterface.JUIsetOnChangeRecordingStatus(true);

        mGrabRunnable.Reset(filePath);
        mGrabThread = new Thread(mGrabRunnable);
        mGrabThread.start();
    }
    public void onStopRecordingNS(){
        JUIInterface.JUIsetOnChangeRecordingStatus(false);

        mGrabRunnable.Stop();
        try {
            mGrabThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
            Log.e(TAG, "=====onStopRecordingNS: " );
        } finally {
            mGrabThread = null;
        }
    }
    public void onSizeChanged(int width, int height){
        mGrabRunnable.onSizeChanged(width, height);
    }
    public static void onViewportSizeChanged(int width, int height){
        mGrabRunnable.onSizeChanged(width, height);
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
    private static class JNIFrameGrabber implements Runnable{
        private VideoWriter videoWriter = null;

        int mFrames = 0;
        boolean isRunning = true;
        Mat mRGBMat;
//        int mWidth = 480, mHeight = 640;
        Size mSize, mSize_origin;
        final float mFactor = 0.5f;
        final double FPS = 30.0;

        public void onSizeChanged(int width, int height){
            mSize_origin = new Size(width, height);
            mSize = new Size(width * mFactor, height * mFactor);
            if(mRGBMat != null) mRGBMat.release();
        }
        public void Reset(String filePath){
            mRGBMat = new Mat(mSize_origin, CvType.CV_8UC4);

            if(videoWriter == null)
                videoWriter = new VideoWriter(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, mSize);
            videoWriter.open(filePath, VideoWriter.fourcc('M','J','P','G'), FPS, mSize);

            mFrames = 0;
            isRunning = true;
        }
        @Override
        public void run() {
            while(isRunning){
                if(!videoWriter.isOpened()){
                    Log.e(TAG, "======debugVideoWriter: fail to open" );
                }else{
//                    int w = 480, h=640;
//                    byte[] data = new byte[w*h * 4];
//                    Arrays.fill(data, (byte)255);
                    mRGBMat.put(0, 0, JNIInterface.JNIgetFrameData());
                    //Changing the orientation of an image
                    Core.flip(mRGBMat, mRGBMat, 0);
                    Mat resized_img = new Mat();
                    Imgproc.resize(mRGBMat, resized_img, mSize);
                    videoWriter.write(resized_img);
                }
                mFrames++;
            }
            videoWriter.release();
            mRGBMat.release();
        }
        public void Stop(){
            isRunning = false;
        }
    }
}

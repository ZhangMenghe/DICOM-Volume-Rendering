package helmsley.vr.Utils;

import android.media.MediaRecorder;
import android.util.Log;

import java.io.IOException;

public class AudioRecorder {
    private static final String TAG = "AudioRecorder";
    private MediaRecorder recorder = null;

    public void onStartRecording(String filePath){
        recorder = new MediaRecorder();
        recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        recorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        recorder.setOutputFile(filePath);
        recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);

        try {
            recorder.prepare();
        } catch (IOException e) {
            Log.e(TAG, "====Audio prepare() failed");
        }
        recorder.start();
    }
    public void onStopRecording() {
        recorder.stop();
        recorder.release();
        recorder = null;
    }
}

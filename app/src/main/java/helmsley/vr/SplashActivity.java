package helmsley.vr;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;

public class SplashActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        Handler handler = new Handler();
//        handler.postDelayed(new Runnable() {
//            public void run() {
//                // yourMethod();
//                startActivity(new Intent(SplashActivity.this, MainActivity.class));
//                finish();
//            }
//        }, 2000);   //2 seconds
        startActivity(new Intent(SplashActivity.this, MainActivity.class));
        finish();
    }
}

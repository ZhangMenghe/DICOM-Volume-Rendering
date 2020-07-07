package helmsley.vr.proto;

import android.util.Log;

import java.text.DecimalFormat;

import io.grpc.stub.StreamObserver;

public class operateClient {

    private static GestureOp.Builder gesture_builder;
    private static StreamObserver<commonResponse> observer;
    private static int ops = 0;
    private static long last_timestamp = 0;
    private static DecimalFormat df = new DecimalFormat("0.00");
    private final static int BACH_TIME_MILLS = 100;
    operateClient(){
        gesture_builder = GestureOp.newBuilder();
        observer = new StreamObserver<commonResponse>() {
            @Override
            public void onNext(commonResponse value) {
            }

            @Override
            public void onError(Throwable t) {
            }

            @Override
            public void onCompleted() {
            }
        };
    }
    public static void setGestureOp(GestureOp.OPType type, float x, float y){
        GestureOp req = gesture_builder.setGid(System.currentTimeMillis()).setType(type).setX(x).setY(y).build();
        rpcManager.operate_stub.setGestureOp(req,observer);
//        ops++;
//        if(System.currentTimeMillis() - last_timestamp > BACH_TIME_MILLS){
//            ops = 0;
//            last_timestamp = System.currentTimeMillis();
//        }
    }
}

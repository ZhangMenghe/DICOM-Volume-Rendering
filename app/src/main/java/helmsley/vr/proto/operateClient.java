package helmsley.vr.proto;

import io.grpc.stub.StreamObserver;

public class operateClient {
    private static GestureOp.Builder gesture_builder;
    private static StreamObserver<commonResponse> observer;
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
        GestureOp req = gesture_builder.setType(type).setX(x).setY(y).build();
        rpcManager.operate_stub.setGestureOp(req,observer);
    }
}

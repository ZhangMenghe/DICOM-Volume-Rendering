package helmsley.vr.proto;

import java.io.PrintWriter;
import java.io.StringWriter;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class rpcManager {
    final static String TAG = "rpcManager";
    final static int CLIENT_ID = 1;

    static ManagedChannel mChannel;
    static dataTransferGrpc.dataTransferBlockingStub data_stub;
    static dataTransferGrpc.dataTransferStub mask_stub;
    static inspectorSyncGrpc.inspectorSyncStub operate_stub;

    public rpcManager(){

    }
    public String Setup(String host, String portStr){
        try{
            mChannel = ManagedChannelBuilder.forAddress(host, Integer.valueOf(portStr)).usePlaintext().build();
            data_stub = dataTransferGrpc.newBlockingStub(mChannel);
            mask_stub = dataTransferGrpc.newStub(mChannel);
            operate_stub = inspectorSyncGrpc.newStub(mChannel);
            return "";
        }catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            pw.flush();
            return String.format("Failed... : %n%s", sw);
        }
    }
    //getters


}

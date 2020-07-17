#include "rpcHandler.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using namespace helmsley;

rpcHandler::rpcHandler(const std::string& host){
    auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
    stub_ = inspectorSync::NewStub(channel);
}
FrameUpdateMsg rpcHandler::getUpdates(){
	ClientContext context;
    stub_->getUpdates(&context, req, &update_msg);
    return update_msg;
}
const RPCVector<GestureOp> rpcHandler::getOperations(){
	ClientContext context;
    std::vector<GestureOp> op_pool;
    OperationBatch op_batch;
    stub_->getOperations(&context,req, &op_batch);
    return op_batch.gesture_op();
}
void rpcHandler::Run(){
    while(true){
        if(ui_ == nullptr || manager_==nullptr || vr_ == nullptr || loader_==nullptr) continue;
        auto msg = getUpdates();
        int gid = 0, tid = 0, cid = 0;
        bool gesture_finished = false;

        for(auto type: msg.types()){
            switch(type){
                case FrameUpdateMsg_MsgType_GESTURE:
                    if(!gesture_finished){
                        tackle_gesture_msg(msg.gestures());
                        gesture_finished = true;
                    }
                    break;
                case FrameUpdateMsg_MsgType_TUNE:
                    tack_tune_msg(msg.tunes().Get(tid++));
                    break;
                case FrameUpdateMsg_MsgType_CHECK:
                    ui_->setCheck(msg.checks().Get(cid++));
                    break;
                case FrameUpdateMsg_MsgType_MASK:
                    ui_->setMaskBits(msg.mask_value());
                    break;
                case FrameUpdateMsg_MsgType_RESET:
                    tackle_reset_msg(msg.reset_value());
                    break;
                case FrameUpdateMsg_MsgType_DATA:
                    tackle_volume_msg(msg.data_value());
                    break;
                default:
                    std::cout<<"UNKNOWN TYPE"<<std::endl;
                    break;
            }
        }
    }
}
void rpcHandler::tackle_gesture_msg(const RPCVector<helmsley::GestureOp> ops){
    for(auto op:ops){
        switch (op.type()){
            case GestureOp_OPType_TOUCH_DOWN:
                vr_->onSingleTouchDown(op.x(), op.y());
                // sp.notify();
                break;
            case GestureOp_OPType_TOUCH_MOVE:
                vr_->onTouchMove(op.x(), op.y());
                // sp.notify();
                break;
            case GestureOp_OPType_SCALE:
                vr_->onScale(op.x(), op.y());
                // sp.notify();
                break;
            case GestureOp_OPType_PAN:
                vr_->onPan(op.x(), op.y());
                // sp.notify();
                break;
            default:
                break;
        }
    }
}
void rpcHandler::tack_tune_msg(helmsley::TuneMsg msg){
    google::protobuf::RepeatedField<float> f;
    switch (msg.type()){
        case TuneMsg_TuneType_ADD_ONE:
            f = msg.values();
            ui_->addTuneParams(std::vector<float> (f.begin(), f.end()));
            break;
        case TuneMsg_TuneType_REMOVE_ONE:
            ui_->removeTuneWidgetById(msg.target());
            break;
        case TuneMsg_TuneType_REMOTE_ALL:
            ui_->removeAllTuneWidget();
            break;
        case TuneMsg_TuneType_SET_ONE:
            ui_->setTuneParamById(msg.target(), msg.sub_target(), msg.value());
            break;
        case TuneMsg_TuneType_SET_ALL:
            f = msg.values();
            ui_->setAllTuneParamById(msg.target(), std::vector<float> (f.begin(), f.end()));
            break;
        case TuneMsg_TuneType_SET_VISIBLE:
            ui_->setTuneWidgetVisibility(msg.target(), (msg.value()>0)?true:false);
            break;
        case TuneMsg_TuneType_SET_TARGET:
            ui_->setTuneWidgetById(msg.target());
            break;
        case TuneMsg_TuneType_CUT_PLANE:
            ui_->setCuttingPlane(msg.target(), msg.value());
            break;
        case TuneMsg_TuneType_COLOR_SCHEME:
            ui_->setColorScheme(msg.target());
            break;
        default:
            break;
    }
}
void rpcHandler::tackle_volume_msg(helmsley::volumeConcise msg){
	//reset data
	auto dims = msg.dims();
	auto ss = msg.size();
	loader_->setupDCMIConfig(dims[0], dims[1], dims[2],ss[0],ss[1],ss[2], msg.with_mask());

	std::cout<<msg.vol_path()<<std::endl;
	if(!loader_->loadData(DATA_PATH + msg.vol_path()+"/data", DATA_PATH + msg.vol_path()+"/mask")){
		std::cout<<"file not exist"<<std::endl;
		return;
	}
		//todo: request from server
	Manager::new_data_available = true;
}
void rpcHandler::tackle_reset_msg(helmsley::ResetMsg msg){
	manager_->onReset();
	ui_->onReset(msg);
}


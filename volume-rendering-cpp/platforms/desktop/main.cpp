#include <platforms/platform.h>
#include <vrController.h>
#include <overlayController.h>

#include "utils/dicomLoader.h"
#include "utils/uiController.h"
#include "utils/fileLoader.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <proto/common.grpc.pb.h>
#include <proto/common.pb.h>
#include <proto/inspectorSync.grpc.pb.h>
#include <proto/inspectorSync.pb.h>
#include <proto/transManager.grpc.pb.h>
#include <proto/transManager.pb.h>
#include <chrono>
#include <iomanip>
#include <cstdio>
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using namespace helmsley;
using namespace std::chrono;
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <unistd.h>
using namespace std;
using namespace helmsley;
template<class T>
using RPCVector = google::protobuf::RepeatedPtrField<T>;
//todo: try to implement operation merge
class semaphore{
public:

  semaphore(int count_ = 0) : count{count_}
  {}

  void notify()
  {
    unique_lock<mutex> lck(mtx);
    ++count;
    cv.notify_one();
  }

  void wait()
  {
	// std::cout<<"count "<< count<<std::endl;
    // unique_lock<mutex> lck(mtx);
    // while(count == 0)
    // {
    //   cv.wait(lck);
    // }
	count = 0;
    // --count;
  }

private:

  mutex mtx;
  condition_variable cv;
  int count;
};
class syncClient{
public:
	syncClient(std::shared_ptr<Channel> channel)
      : stub_(inspectorSync::NewStub(channel)) {
		//   mc = &controller;//std::unique_ptr<vrController>(controller);
	  }
	
	FrameUpdateMsg getUpdates(){
	ClientContext context;
		stub_->getUpdates(&context, req, &update_msg);
		return update_msg;
	}
	const RPCVector<GestureOp> getOperations(){
	ClientContext context;

		std::vector<GestureOp> op_pool;

		OperationBatch op_batch;
		stub_->getOperations(&context,req, &op_batch);
		// std::cout<<op_batch.bid()<<std::endl;
		// for(auto op: op_batch.gesture_op())
		// 	op_pool.push_back(op);
		return op_batch.gesture_op();

	// 	OperationResponse feature;

	// 	std::unique_ptr<ClientReader<OperationResponse> > reader(
    //     stub_->getOperations(&context, req));
    // while (reader->Read(&feature))
	// 	op_pool.push_back(feature.gesture_op());
	// 	return op_pool;
		// return feature.gesture_op();
	}  
private:
  	std::unique_ptr<inspectorSync::Stub> stub_;
	Request req;
	FrameUpdateMsg update_msg;
};

GLFWwindow* window;
dicomLoader loader_;
uiController ui_;
//order matters
Manager manager_;
vrController controller_;
syncClient* rpc_manager;
// semaphore sp;
// std::mutex mtx;

bool is_pressed = false;
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	if(is_pressed){
		controller_.onTouchMove(float(xpos), float(ypos));
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT){
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
        switch (action){
			case GLFW_PRESS:
				controller_.onSingleTouchDown(float(xpos), float(ypos));
				is_pressed = true;
				break;
			case GLFW_RELEASE:
				is_pressed = false;
			default:
				break;
			}
    }
}

void onCreated(){
	ui_.InitAll();
	controller_.onViewCreated();
	overlayController::instance()->onViewCreated();
	ui_.AddTuneParams();

	// 430, 768,
	overlayController::instance()->setOverlayRect(0, 430, 85, 0, 310);
    overlayController::instance()->setOverlayRect(1, 430, 36, 0, 295);

	//load data
	if(loader_.loadData("helmsley_cached/Larry_Smarr_2016/series_23_Cor_LAVA_PRE-Amira/data", "helmsley_cached/Larry_Smarr_2016/series_23_Cor_LAVA_PRE-Amira/mask")){
	// if(loader_.loadData("dicom-images/sample_data_2bytes_2012", LOAD_DICOM)){
        controller_.assembleTexture(2, 512,512,144, -1, -1, -1, loader_.getVolumeData(), loader_.getChannelNum());
		loader_.reset();
	}
}
void onDraw(){
	controller_.onDraw();
	if(controller_.isDrawing()) overlayController::instance()->onDraw();
}
void onViewChange(int width, int height){
	manager_.onViewChange(width, height);
	controller_.onViewChange(width, height);
	overlayController::instance()->onViewChange(width, height);
}
void onDestroy(){
}

bool InitWindow(){
	// Initialise GLFW
	if( !glfwInit() ){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(430, 768, "Volume Rendering", nullptr, nullptr);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	// gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	onViewChange(430,768);
	return true;
}

void setupApplication(){
	int dims = 144;
	loader_.setupDCMIConfig(512,512,dims,true);
}

void tackle_gesture_msg(const RPCVector<GestureOp> ops){
		// auto ops = rpc_manager->getOperations();
	for(auto op:ops){
		switch (op.type()){
		case GestureOp_OPType_TOUCH_DOWN:
			controller_.onSingleTouchDown(op.x(), op.y());
			// sp.notify();
			break;
		case GestureOp_OPType_TOUCH_MOVE:
			controller_.onTouchMove(op.x(), op.y());
			// sp.notify();
			break;
		case GestureOp_OPType_SCALE:
			controller_.onScale(op.x(), op.y());
			// sp.notify();
			break;
		case GestureOp_OPType_PAN:
			controller_.onPan(op.x(), op.y());
			// sp.notify();
			break;
		default:
			break;
		}
	}
}
void tack_tune_msg(TuneMsg msg){

}

void tackle_reset_msg(ResetMsg msg){
	manager_.onReset();
	//init check paramd
}
void tackle_update_msg(){
	auto msg = rpc_manager->getUpdates();
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
				ui_.setCheck(msg.checks().Get(cid++));
				break;
			case FrameUpdateMsg_MsgType_MASK:
				ui_.setMaskBits(msg.mask_value());
				break;
			case FrameUpdateMsg_MsgType_RESET:
				tackle_reset_msg(msg.reset_value());
				break;
			default:
				std::cout<<"UNKNOWN TYPE"<<std::endl;
				break;
		}
	}
}
void rpc_thread(){
	// auto last_ts = std::chrono::system_clock::now();
	// size_t ops_count = 0;
	rpc_manager = new syncClient(grpc::CreateChannel(
      "localhost:23333", grpc::InsecureChannelCredentials()));
	while(true) tackle_update_msg();

		// auto duration = std::chrono::system_clock::now() - last_ts;
		// auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
		// if (seconds > 2) {
		// 	// std::cerr << "ops: " << ops_count << std::endl;
		// 	last_ts = std::chrono::system_clock::now();
		// 	ops_count = 0;
		// }
		// std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
void main_thread(){
	setupShaderContents(&controller_);
	
	if(!InitWindow()) return;

	setupApplication();
	onCreated();

	do{
		// sp.wait();
		// std::cout<<"draw"<<std::endl;
		// usleep(500);
		onDraw();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
	
	onDestroy();

	glfwDestroyWindow(window);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

int main(int argc, char** argv){
	std::thread t1(&main_thread);
	std::thread t2(&rpc_thread);

	t1.join();
	t2.join();
	return 0;
}
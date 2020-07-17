#include <platforms/platform.h>
#include <vrController.h>
#include <overlayController.h>

#include "utils/dicomLoader.h"
#include "utils/uiController.h"
#include "utils/fileLoader.h"


#include <chrono>
#include <iomanip>
#include <cstdio>

using namespace std::chrono;
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <unistd.h>
using namespace std;

GLFWwindow* window;
dicomLoader loader_;
uiController ui_;
//order matters
Manager manager_;
vrController controller_;
#ifdef RPC_ENABLED
#include <RPCs/rpcHandler.h>
rpcHandler* rpc_handler;
std::thread* rpc_thread;
#endif
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	controller_.onDraw();
	if(controller_.isDrawing()) overlayController::instance()->onDraw();
	if(Manager::new_data_available){
		Manager::new_data_available = false;
		loader_.startToAssemble(&controller_);
		loader_.reset();
	}
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
	loader_.setupDCMIConfig(512,512,dims,-1,-1,-1,true);
}

int main(int argc, char** argv){
	setupShaderContents(&controller_);
	
	if(!InitWindow()) return -1;

	setupApplication();
	onCreated();
	#ifdef RPC_ENABLED
	rpc_handler = new rpcHandler("localhost:23333");
    rpc_thread = new thread(&rpcHandler::Run, rpc_handler);
	rpc_handler->setManager(&manager_);
	rpc_handler->setUIController(&ui_);
	rpc_handler->setVRController(&controller_);
	rpc_handler->setDataLoader(&loader_);
	#endif
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
	return 0;
}
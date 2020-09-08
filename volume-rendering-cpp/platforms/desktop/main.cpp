#include <platforms/platform.h>
#include <vrController.h>
#include <overlayController.h>
#include <dicomRenderer/centerLineRenderer.h>
#include <dicomRenderer/screenQuad.h>

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
#include <platforms/desktop/common.h>
// std::string ds_path = "dicom-data/IRB02/21_WATERPOSTCORLAVAFLEX20secs/";
// std::string ds_path = "dicom-data/IRB03/22_WATERPOSTCORLAVAFLEX20secs/";
// std::string ds_path = "dicom-data/IRB04/21_WATERPOSTCORLAVAFLEX20secs/";
// std::string ds_path = "dicom-data/IRB05/17_WATERPOSTCORLAVAFLEX20secs/";
// std::string ds_path = "dicom-data/IRB06/19_WATERPOSTCORLAVAFLEX20secs/";

std::string cline_fname ="centerline.txt";
std::string ds_path = "dicom-data/IRB01/2100_FATPOSTCORLAVAFLEX20secs/";
// glm::vec3 vol_dims = glm::vec3(512,512,224);
// glm::vec3 vol_dims = glm::vec3(512,512,172);

// glm::vec3 vol_dims = glm::vec3(512,512,216);
glm::vec3 vol_dims = glm::vec3(512,512,164);

// float cline_data[2][4000 * 3] = {.0f};
std::vector<float*> cline_data;

bool pre_draw = true;
bool is_pressed = false;
int cid = 2;
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
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	float f = 1.0 + yoffset * 0.1f;
	controller_.onScale(f, f);
}
// void set_centerline_cutting(int id, int gap){
// 	id = fmax(id, gap);
// 	id = fmin(id,3999-gap);

// 	float * data = cline_data[0];
// 	glm::vec3 pp = glm::vec3(data[3*id],data[3*id+1],data[3*id+2]);
// 	glm::vec3 pn = glm::vec3(data[3*(id+gap)], data[3*(id+gap)+1], data[3*(id+gap)+2]) - glm::vec3(data[3*(id-gap)], data[3*(id-gap)+1], data[3*(id-gap)+2]);
// 	if(glm::dot(pn,glm::vec3(0,0,-1)) < .0f)pn=-pn;
// 	controller_.setCuttingPlane(pp, pn);
// }
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action==GLFW_RELEASE) return;
	switch (key)
	{
	case GLFW_KEY_W:
		ui_.setCheck("Wireframe", !Manager::param_bool[dvr::CHECK_POLYGON_WIREFRAME]);
		break;
	case GLFW_KEY_P:
		ui_.setCheck("Mesh", !Manager::param_bool[dvr::CHECK_DRAW_POLYGON]);
		break;
	case GLFW_KEY_V:
		ui_.setCheck("Volume", !Manager::param_bool[dvr::CHECK_VOLUME_ON]);
		break;
	case GLFW_KEY_C:
		ui_.setCheck("Cutting", !Manager::param_bool[dvr::CHECK_CUTTING]);
		if(Manager::param_bool[dvr::CHECK_CUTTING]){
			ui_.setCheck("Center Line Travel",false);
			std::cout<<"switch to cutting plane mode"<<std::endl;
			controller_.SwitchCuttingPlane(dvr::CUT_CUTTING_PLANE);
		}
		break;
	case GLFW_KEY_T:
		ui_.setCheck("Center Line Travel", !Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]);
		//reset to start of 
		if(Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]){
			// set_centerline_cutting(0, 2);
			ui_.setCheck("Cutting", false);
			std::cout<<"switch to traversal mode"<<std::endl;
			controller_.SwitchCuttingPlane(dvr::CUT_TRAVERSAL);
		}
		break;
	case GLFW_KEY_N:
		if(Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]) {
			// set_centerline_cutting((cid%4000),1);cid+=5;
			controller_.setCuttingPlane(0, 5);
			}
		break;
	default:
		break;
	}
}
void get_center_line_points(){
	std::vector<int>ids;

	std::string filename = ds_path + cline_fname;	
	int cidx = 0;
	float* data;// = &cline_data[cidx][0];
    std::ifstream ShaderStream(PATH(filename), std::ios::in);

	if(ShaderStream.is_open()){
		std::string line = "", substr;
		int idx;
		while(getline(ShaderStream, line)){
			if(line.length() < 3){
				cline_data.push_back(new float[4000*3]);
				data = cline_data.back();
				ids.push_back(std::stoi(line));
				idx = 0;
				continue;
			}
			std::stringstream ss(line);
			while(ss.good()){
				getline(ss,substr,',');
				data[idx++] = std::stof(substr);
			}
		}
		ShaderStream.close();

		for(int i=0;i<2;i++){
			controller_.setupCenterLine(ids[i], cline_data[i]);
		}
	}else{
		LOGE("====Failed to load file: %s", filename);
	}
}
void onCreated(){
	ui_.InitAll();
	controller_.onViewCreated();
	overlayController::instance()->onViewCreated();

	ui_.AddTuneParams();

	// 430, 768,
	overlayController::instance()->setOverlayRect(0, 430, 85, 0, 310);
    overlayController::instance()->setOverlayRect(1, 430, 36, 0, 275);

	//load data
	if(loader_.loadData(ds_path +"data", ds_path+"mask")){
	// if(loader_.loadData("dicom-images/sample_data_2bytes_2012", LOAD_DICOM)){
        controller_.assembleTexture(2, vol_dims.x, vol_dims.y, vol_dims.z, -1, -1, -1, loader_.getVolumeData(), loader_.getChannelNum());
		loader_.reset();
	}
	get_center_line_points();

	// 	//load data
	// if(loader_.loadData(ds_path+"mask_simple",128,128,41)){
    //     controller_.setupSimpleMaskTexture(128,128,41, loader_.getSimpleMask());
	// 	loader_.reset();
	// }
}
void onDraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	controller_.onDraw();
	if(controller_.isDrawing()) overlayController::instance()->onDraw();
	
	// if(Manager::new_data_available){
	// 	Manager::new_data_available = false;
	// 	loader_.startToAssemble(&controller_);
	// 	loader_.reset();
	// }
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
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	onViewChange(430,768);
	return true;
}

void setupApplication(){
	loader_.setupDCMIConfig(vol_dims.x, vol_dims.y, vol_dims.z, -1,-1,-1,true);
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


#include <platforms/platform.h>
#include <vrController.h>
#include "utils/dicomLoader.h"
#include "utils/uiController.h"
#include "utils/fileLoader.h"
GLFWwindow* window;
dicomLoader loader_;
uiController ui_;
vrController controller_;
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
	setupShaderContents(&controller_);
	ui_.InitTuneParam();
	ui_.InitCheckParam();
	controller_.onViewCreated();

	//load data
	if(loader_.loadData("helmsley_cached/Larry-2016-10-26-MRI/series_214_DYN_COR_VIBE_3_RUNS/data", "helmsley_cached/Larry-2016-10-26-MRI/series_214_DYN_COR_VIBE_3_RUNS/mask")){
	// if(loader_.loadData("dicom-images/sample_data_2bytes_2012", LOAD_DICOM)){
		controller_.assembleTexture(loader_.getVolumeData(), loader_.getChannelNum());
		loader_.reset();
	}
}
void onDraw(){
	controller_.onDraw();
}
void onViewChange(int width, int height){
	controller_.onViewChange(width, height);
}
void onDestroy(){
	controller_.onDestroy();
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
	window = glfwCreateWindow( 430, 768, "Volume Rendering", nullptr, nullptr);
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
	controller_.setVolumeConfig(512,512,dims);
	// if(loader_.loadData("dicom-images/sample_data_2bytes", LOAD_DICOM, 2)){
	// 	controller_.assembleTexture(loader_.getVolumeData());
	// 	loader_.reset();
	// }

}

int main(int argc, char** argv){
	
	if(!InitWindow()) return -1;

	setupApplication();
	onCreated();

	do{
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


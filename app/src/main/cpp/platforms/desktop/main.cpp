
#include <stdlib.h>
#include <string>
#include <iostream>
#include <platforms/platform.h>
#include <GLPipeline/Shader.h>
#include <vrController.h>

GLFWwindow* window;
Shader shader_;
// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include "utils/dicomLoader.h"
#include "utils/uiController.h"

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

bool onCreated(){
	ui_.InitTuneParam();
	ui_.InitCheckParam();
	controller_.onViewCreated();
	
	// onViewChange(1024, 768);
	controller_.onViewChange(1024, 768);

	return true;

}
void onDraw(){
	controller_.onDraw();
}
void onViewChange(int width, int height){
	controller_.onViewChange(width, height);
}
void onDestroy(){
	// controller_.onDestroy(true);
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
	window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
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

	return true;
}

void setupApplication(){
	loader_.setupDCMIConfig(512,512,48);
	controller_.setVolumeConfig(512,512,48);
	if(loader_.loadDicomFiles("dicom-images/sample_data_4bytes")){
		controller_.assembleTexture(loader_.getVolumeData());
		loader_.reset();
	}
}

int main(int argc, char** argv){
	
	if(!InitWindow()) return -1;

	setupApplication();
	if(!onCreated()) return -1;

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


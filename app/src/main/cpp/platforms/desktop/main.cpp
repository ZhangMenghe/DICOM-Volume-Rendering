
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


bool onCreated(){
	controller_.onViewCreated();
	return true;

}
void onDraw(){
	// controller_.onDraw();
}
void onViewChange(int width, int height){
	// controller_.onViewChange(width, height);
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
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	onViewChange(1024, 768);
	return true;
}

void setupApplication(){
	loader_.setupDCMIConfig(512,512,48);
	controller_.setVolumeConfig(512,512,48);
	if(loader_.loadDicomFiles("dicom-images/sample_data")){
		controller_.assembleTexture(loader_.getVolumeData());
		loader_.reset();
	}
	ui_.InitTuneParam();
	ui_.InitCheckParam();
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
	
	// // Dark blue background
	// glClearColor(0.0f, 0.0f, 0.4f, 0.0f);



	// GLuint VertexArrayID;
	// glGenVertexArrays(1, &VertexArrayID);
	// glBindVertexArray(VertexArrayID);

	// // Create and compile our GLSL program from the shaders
	// // GLuint programID = LoadShaders( "shaders/naive.vert", "shaders/naive.frag" );
	
	// if(!shader_.AddShader(GL_VERTEX_SHADER, loadTextFile("shaders/naive.vert")) 
	// || !shader_.AddShader(GL_FRAGMENT_SHADER, loadTextFile("shaders/naive.frag"))
	// ||!shader_.CompileAndLink())
	// 	return 0;

	

	// static const GLfloat g_vertex_buffer_data[] = { 
	// 	-1.0f, -1.0f, 0.0f,
	// 	 1.0f, -1.0f, 0.0f,
	// 	 0.0f,  1.0f, 0.0f,
	// };

	// GLuint vertexbuffer;
	// glGenBuffers(1, &vertexbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// do{

	// 	// Clear the screen
	// 	glClear( GL_COLOR_BUFFER_BIT );

	// 	// Use our shader
	// 	// glUseProgram(programID);
	// 	shader_.Use();

	// 	// 1rst attribute buffer : vertices
	// 	glEnableVertexAttribArray(0);
	// 	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// 	glVertexAttribPointer(
	// 		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	// 		3,                  // size
	// 		GL_FLOAT,           // type
	// 		GL_FALSE,           // normalized?
	// 		0,                  // stride
	// 		(void*)0            // array buffer offset
	// 	);

	// 	// Draw the triangle !
	// 	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	// 	glDisableVertexAttribArray(0);

	// 	// Swap buffers
	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();

	// } // Check if the ESC key was pressed or the window was closed
	// while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
	// 	   glfwWindowShouldClose(window) == 0 );

	// // Cleanup VBO
	// glDeleteBuffers(1, &vertexbuffer);
	// glDeleteVertexArrays(1, &VertexArrayID);
	// // glDeleteProgram(programID);
	// shader_.UnUse();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


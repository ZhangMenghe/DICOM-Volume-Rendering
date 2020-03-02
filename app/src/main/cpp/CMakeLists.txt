cmake_minimum_required(VERSION 3.4.1)

project (vrDesktop)

# SET( GCC_COVERAGE_LINK_FLAGS    "-v")
# SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
# SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
# SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_STATIC_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(GLM_DIRECTORY /usr/local/include/glm)

set(OpenGL_LIBS
	glfw	
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARY}
	)
set(VRDESKTOP_SRCS
	"platforms/desktop/main.cpp")


SET(GL_PIPELINE_FILES
	GLPipeline/Shader.cpp
	GLPipeline/Mesh.cpp
	GLPipeline/Texture.cpp)
SET(DICOM_RENDERERS
	dicomRenderer/funcsRenderer.cpp
	dicomRenderer/raycastRenderer.cpp
	dicomRenderer/texturebasedRenderer.cpp
	dicomRenderer/cuttingController.cpp
	dicomRenderer/screenQuad.cpp)
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLM_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DRESOURCE_DESKTOP_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../assets/")
add_library(vrNative
	STATIC
	vrController.cpp
	${DICOM_RENDERERS}
	${GL_PIPELINE_FILES}
	)

target_link_libraries(
	vrNative
	${OpenGL_LIBS}
)

add_executable(
	vrDesktop
	${VRDESKTOP_SRCS}
)
target_link_libraries(
	vrDesktop
	vrNative
)
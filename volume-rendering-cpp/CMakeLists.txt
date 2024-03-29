cmake_minimum_required(VERSION 3.4.1)
project(vrNative)
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_BUILD_TYPE "Debug")
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    Message(STATUS ${GLEW_LIBRARIES})
    Message(STATUS ${OPENGL_LIBRARY})

    set(OpenGL_LIBS
        glfw
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARY}
        )
    set(ASSET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/main/assets)

elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    set(OpenGL_LIBS
        GLESv3
        EGL
        log)
endif()
set(GLM_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glm-0.9.9.7/glm)

SET(GL_PIPELINE_FILES
        GLPipeline/Shader.cpp
        GLPipeline/Mesh.cpp
        GLPipeline/Texture.cpp)
SET(UTILS_FILES
        Utils/perfMonitor.cpp)
SET(DICOM_RENDERERS
        dicomRenderer/organMeshRenderer.cpp
        dicomRenderer/raycastRenderer.cpp
        dicomRenderer/texturebasedRenderer.cpp
        dicomRenderer/centerLineRenderer.cpp
        dicomRenderer/cuttingController.cpp
        dicomRenderer/colorbarRenderer.cpp
        dicomRenderer/graphRenderer.cpp
        dicomRenderer/screenQuad.cpp)

include_directories(${GLM_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(vrNative
        SHARED
        vrController.cpp
        overlayController.cpp
        Manager.cpp
        ${DICOM_RENDERERS}
        ${GL_PIPELINE_FILES}
        ${UTILS_FILES})

target_link_libraries(
        vrNative
        ${OpenGL_LIBS})

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms/desktop)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms/android)
endif()
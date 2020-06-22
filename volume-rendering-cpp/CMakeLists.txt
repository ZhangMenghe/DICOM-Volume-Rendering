cmake_minimum_required(VERSION 3.4.1)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_CXX_STANDARD 14)
#    set(CMAKE_BUILD_TYPE "Debug")
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
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
    include_directories(${ARCORE_INCLUDE})
endif()
set(GLM_DIRECTORY /usr/local/include/glm)

SET(GL_PIPELINE_FILES
        GLPipeline/Shader.cpp
        GLPipeline/Mesh.cpp
        GLPipeline/Texture.cpp)
#SET(UTILS_FILES
#        Utils/perfMonitor.cpp
#        )
SET(DICOM_RENDERERS
        dicomRenderer/raycastRenderer.cpp
        dicomRenderer/texturebasedRenderer.cpp
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
#        ${UTILS_FILES}
        )

target_link_libraries(
        vrNative
        ${OpenGL_LIBS}
)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms/desktop)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms/android)
endif()


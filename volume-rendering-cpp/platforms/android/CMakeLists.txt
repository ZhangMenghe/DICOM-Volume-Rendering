cmake_minimum_required(VERSION 3.4.1)

SET(KERNEL_LIBRARIES
        jnigraphics
        log
        m)

SET(JNI_FILES
        Jnis/jni_interface.cpp
        Jnis/jui_interface.cpp
        )
SET(AR_FILES
        ARHelpers/arController.cpp
        )
SET(UTIL_FILES
        Utils/assetLoader.cpp
        #        ARHelpers/jni_interface.cpp
        )
SET(RENDER_FILES
        Renderers/PlaneRenderer.cpp
        Renderers/PointCloudRenderer.cpp
        Renderers/backgroundRenderer.cpp)
# Import the ARCore library.
add_library(arcore SHARED IMPORTED)
set_target_properties(
        arcore PROPERTIES IMPORTED_LOCATION
        ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so
        INTERFACE_INCLUDE_DIRECTORIES ${ARCORE_INCLUDE}
        )
include_directories(${GLM_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(vrAndroid
        SHARED
        ${JNI_FILES}
        ${AR_FILES}
        ${RENDER_FILES}
        ${UTIL_FILES}
        )

target_link_libraries(
        vrAndroid
        vrNative
        android
        mediandk
        arcore
        GLESv2
        ${OpenGL_LIBS}
        ${KERNEL_LIBRARIES})
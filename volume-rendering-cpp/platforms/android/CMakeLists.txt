cmake_minimum_required(VERSION 3.4.1)

SET(KERNEL_LIBRARIES
        jnigraphics
        log
        m)
SET(UTILS
        Utils/assetLoader.cpp
        )
SET(JNI_FILES
        Jnis/jni_interface.cpp
        Jnis/jui_interface.cpp
        )
SET(AR_FILES
        ARHelpers/arController.cpp
        ARHelpers/ArucoMarkerTracker.cpp
        )
SET(RENDER_FILES
        Renderers/cutplaneRenderer.cpp
        Renderers/lineRenderer.cpp
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
# Import opencv
add_library( lib_opencv SHARED IMPORTED )
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so)

include_directories(${GLM_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(vrAndroid
        SHARED
        ${JNI_FILES}
        ${AR_FILES}
        ${RENDER_FILES}
        ${UTILS})

target_link_libraries(
        vrAndroid
        vrNative
        android
        mediandk
        lib_opencv
        arcore
        GLESv2
        ${OpenGL_LIBS}
        ${KERNEL_LIBRARIES})
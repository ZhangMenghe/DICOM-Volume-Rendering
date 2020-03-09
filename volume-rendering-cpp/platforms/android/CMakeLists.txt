cmake_minimum_required(VERSION 3.4.1)

SET(KERNEL_LIBRARIES
        jnigraphics
        log
        m)
SET(ANDROID_HELPER_FILES
        assetLoader.cpp
        )
SET(JNI_FILES
        Jnis/jni_interface.cpp
        Jnis/jui_interface.cpp
        )
include_directories(${GLM_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(vrAndroid
        SHARED
        ${JNI_FILES}
        ${ANDROID_HELPER_FILES}

        )

target_link_libraries(
        vrAndroid
        vrNative
        android
        mediandk
        ${KERNEL_LIBRARIES})
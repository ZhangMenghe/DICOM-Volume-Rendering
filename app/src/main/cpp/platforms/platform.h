#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#ifdef __ANDROID__
    // Include the latest possible header file( GL version header )
    #if __ANDROID_API__ >= 24
    #include <GLES3/gl32.h>
    #elif __ANDROID_API__ >= 21
    #include <GLES3/gl31.h>
    #else
    #include <GLES3/gl3.h>
    #endif

    #include <android/log.h>
 
    #define TAG "ANDROID-HELPER"
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

#else
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
    #include <stdio.h>
    #include <string>


    #define LOGE(...) printf(__VA_ARGS__)
    #define LOGI(...) printf(__VA_ARGS__)
    #define STRING(...) std::string(__VA_ARGS__)

#endif

#endif
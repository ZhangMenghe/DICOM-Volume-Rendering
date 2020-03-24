#ifndef JNI_MAIN_H
#define JNI_MAIN_H

#include <jni.h>
#include <string>
#include <vector>
#include "nEntrance.h"

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_JNIInterface_##funcName

#define JUI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_DUIs_JUIInterface_##funcName

namespace dvr{
    // maintain a reference to the JVM so we can use it later.
    inline static JavaVM *g_vm = nullptr;

    inline jlong nativeAddr;

    inline jlong getNativeClassAddr(nEntrance * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline nEntrance * nativeApp(jlong ptr){
        return reinterpret_cast<nEntrance *>(ptr);
    }

    inline std::string jstring2string(JNIEnv *env, jstring jStr){
        const char *cstr = env->GetStringUTFChars(jStr, NULL);
        std::string str = std::string(cstr);
        env->ReleaseStringUTFChars(jStr, cstr);
        return str;
    }
};
#endif
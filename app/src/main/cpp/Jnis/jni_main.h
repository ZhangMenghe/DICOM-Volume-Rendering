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
        Java_helmsley_vr_UIsController_##funcName

#define DUI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_DUIs_JUIInterface_##funcName

namespace dvr{
    inline jlong nativeAddr;

    enum PARAM_BOOL{
        CHECK_RAYCAST=0,
        CHECK_COLOR_TRANS,
        CHECK_CUTTING,
        CHECK_MASKON,
        CHECK_FREEZE_VOLUME,
        CHECK_FREEZE_CPLANE
    };
    enum PARAM_TEX{
        TUNE_OVERALL=0,
        TUNE_LOWEST,
        TUNE_CUTOFF,
        TUNE_CUTTING_TEX
    };
    enum PARAM_RAYCAST{
        TUNE_DENSITY = 0,
        TUNE_CONTRAST,
        TUNE_BRIGHT,
        TUNE_CUTTING_RAY
    };
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
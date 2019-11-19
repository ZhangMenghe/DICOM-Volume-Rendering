#ifndef JNI_INTERFACE_H
#define JNI_INTERFACE_H

#include "jni_main.h"

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_JNIInterface_##funcName

extern "C"{
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager);

JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv*, jclass,  jobjectArray, jobjectArray, jint);

JNI_METHOD(void, JNIsendDCMImg)(JNIEnv*, jclass, jint,  jfloat, jbyteArray);

JNI_METHOD(void, JNIsetupDCMIConfig)(JNIEnv*, jclass, jint, jint, jint);

JNI_METHOD(void, JNIAssembleVolume)(JNIEnv*, jclass);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass);

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv * env, jclass);

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint, jint);
}


#endif

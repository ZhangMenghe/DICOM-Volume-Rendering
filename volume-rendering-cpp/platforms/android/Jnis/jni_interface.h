#ifndef JNI_INTERFACE_H
#define JNI_INTERFACE_H

#include "jni_main.h"

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_JNIInterface_##funcName

extern "C"{
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager);

JNI_METHOD(void, JNIsendData)(JNIEnv*, jclass, jint, jint, jint, jint, jbyteArray);

JNI_METHOD(void, JNIsetupDCMIConfig)(JNIEnv*, jclass, jint, jint, jint, jboolean);

JNI_METHOD(void, JNIAssembleVolume)(JNIEnv*, jclass);

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv*, jclass);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass);

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv * env, jclass);

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint, jint, jint);
}


#endif

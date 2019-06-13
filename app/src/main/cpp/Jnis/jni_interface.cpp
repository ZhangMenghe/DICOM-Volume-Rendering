#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"

JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    return (jlong)0;
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv * env, jclass){

}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint, jint){

}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jobject){

}

JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv*, jobject,  jobjectArray, jint){

}
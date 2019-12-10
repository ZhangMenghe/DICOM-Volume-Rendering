#ifndef DUI_INTERFACE_H
#define DUI_INTERFACE_H
#include "jni_main.h"

extern "C"{
//    JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass, jfloat, jfloat);
//    JUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat, jfloat);
//    JUI_METHOD(void, JUIonScale)(JNIEnv *, jclass, jfloat, jfloat);
//    JUI_METHOD(void, JUIonPan)(JNIEnv *, jclass, jfloat, jfloat);
//
//    JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint , jstring);
    DUI_METHOD(void, JUIInitTuneParam)(JNIEnv *, jclass, jint, jint, jobjectArray, jfloatArray);
    DUI_METHOD(void, JUIInitCheckParam)(JNIEnv * env, jclass, jint, jobjectArray, jbooleanArray);

    DUI_METHOD(void, JUIsetTuneParam)(JNIEnv *, jclass, jint, jstring , jfloat);
    DUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring, jboolean);




//
//    JUI_METHOD(float, JUIgetFPS)(JNIEnv * env, jclass);
}


#endif

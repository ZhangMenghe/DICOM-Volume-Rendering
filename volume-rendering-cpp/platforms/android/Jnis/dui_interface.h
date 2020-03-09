#ifndef DUI_INTERFACE_H
#define DUI_INTERFACE_H
#include "jni_main.h"

extern "C"{
    DUI_METHOD(void, JUIonReset)(JNIEnv*, jclass);
    DUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass, jfloat, jfloat);
    DUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat, jfloat);
    DUI_METHOD(void, JUIonScale)(JNIEnv *, jclass, jfloat, jfloat);
    DUI_METHOD(void, JUIonPan)(JNIEnv *, jclass, jfloat, jfloat);

    DUI_METHOD(void, JUIInitTuneParam)(JNIEnv *, jclass, jint, jint, jobjectArray, jfloatArray);
    DUI_METHOD(void, JUIInitCheckParam)(JNIEnv * env, jclass, jint, jobjectArray, jbooleanArray);

    DUI_METHOD(void, JUIsetTuneParam)(JNIEnv *, jclass, jint, jstring , jfloat);
    DUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring, jboolean);

    DUI_METHOD(void, JUIsetCuttingPlane)(JNIEnv * env, jclass, jint, jfloat, jboolean);
    DUI_METHOD(void, JUIsetMaskBits)(JNIEnv * env, jclass, jint, jint);



//    JUI_METHOD(float, JUIgetFPS)(JNIEnv * env, jclass);
}


#endif

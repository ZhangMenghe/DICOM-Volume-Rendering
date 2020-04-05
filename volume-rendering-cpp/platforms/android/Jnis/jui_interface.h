#ifndef DUI_INTERFACE_H
#define DUI_INTERFACE_H
#include "jni_main.h"

extern "C"{
    JUI_METHOD(void, JUIonReset)(JNIEnv*, jclass, jint, jobjectArray, jbooleanArray);
    JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonScale)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonPan)(JNIEnv *, jclass, jfloat, jfloat);

    JUI_METHOD(void, JUIInitTuneParam)(JNIEnv *, jclass, jint, jint, jobjectArray, jfloatArray);
    JUI_METHOD(void, JUIInitCheckParam)(JNIEnv * env, jclass, jint, jobjectArray, jbooleanArray);

    JUI_METHOD(void, JUIsetTuneParam)(JNIEnv *, jclass, jint, jstring , jfloat);
    JUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring, jboolean);

    JUI_METHOD(void, JUIsetCuttingPlane)(JNIEnv * env, jclass, jint, jfloat);
    JUI_METHOD(void, JUIsetMaskBits)(JNIEnv * env, jclass, jint, jint);
    JUI_METHOD(void, JuisetColorScheme)(JNIEnv * env, jclass, jint);
}


#endif

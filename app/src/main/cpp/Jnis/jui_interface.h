#ifndef JUI_INTERFACE_H
#define JUI_INTERFACE_H
#include "jni_main.h"

extern "C"{
    JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat, jfloat);

    JUI_METHOD(void, JUIsetInitialValueBool)(JNIEnv *, jclass, jstring , jboolean);
    JUI_METHOD(void, JUIsetInitialValueFloat)(JNIEnv *, jclass, jstring , jfloat);
    JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint , jint);
    JUI_METHOD(void, JUIsetParam)(JNIEnv * env, jclass, jint , jfloat);
    JUI_METHOD(void, JUIsetSwitches)(JNIEnv * env, jclass, jint , jboolean);

    JUI_METHOD(float, JUIgetFPS)(JNIEnv * env, jclass);
}


#endif

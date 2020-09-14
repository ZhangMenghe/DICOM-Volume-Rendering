#ifndef DUI_INTERFACE_H
#define DUI_INTERFACE_H
#include "jni_main.h"

extern "C"{
    JUI_METHOD(void, JUIsetAllTuneParamByIdNative)(JNIEnv*, jclass, jint, jfloatArray);
    JUI_METHOD(void, JUIonResetNative)(JNIEnv*, jclass, jint, jobjectArray, jbooleanArray, jfloatArray, jfloatArray);
    JUI_METHOD(void, JUIonSingleTouchDownNative)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonTouchMoveNative)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonScaleNative)(JNIEnv *, jclass, jfloat, jfloat);
    JUI_METHOD(void, JUIonPanNative)(JNIEnv *, jclass, jfloat, jfloat);

    JUI_METHOD(void, JUIaddTuneParamsNative)(JNIEnv *, jclass, jintArray, jfloatArray);
    JUI_METHOD(void, JUIsetTuneWidgetByIdNative)(JNIEnv *, jclass, jint);
    JUI_METHOD(void, JUIremoveTuneWidgetByIdNative)(JNIEnv *, jclass, jint);
    JUI_METHOD(void, JUIremoveAllTuneWidgetNative)(JNIEnv *, jclass);
    JUI_METHOD(void, JUIsetTuneWidgetVisibilityNative)(JNIEnv *, jclass, jint, jboolean);

    JUI_METHOD(void, JUIsetTuneParamByIdNative)(JNIEnv *, jclass, jint, jint, jfloat);
    JUI_METHOD(void, JUIsetDualParamByIdNative)(JNIEnv *, jclass, jint, jfloat, jfloat);
    JUI_METHOD(void, JUIsetChecksNative)(JNIEnv * env, jclass, jstring, jboolean);
    JUI_METHOD(void, JUISwitchCuttingPlaneNative)(JNIEnv * env, jclass, jint);

    JUI_METHOD(jfloatArray, JUIgetVCStatesNative)(JNIEnv * env, jclass);
    JUI_METHOD(void, JUIsetCuttingPlaneNative)(JNIEnv * env, jclass, jfloat);
    JUI_METHOD(void, JUIsetCuttingPlaneDeltaNative)(JNIEnv * env, jclass, jint, jint);
    JUI_METHOD(jfloatArray, JUIgetCuttingPlaneStatusNative)(JNIEnv * env, jclass);
    JUI_METHOD(void, JUIsetMaskBitsNative)(JNIEnv * env, jclass, jint, jint);
    JUI_METHOD(void, JUIsetColorSchemeNative)(JNIEnv * env, jclass, jint);
    JUI_METHOD(void, JUIsetTraversalTargetNative)(JNIEnv * env, jclass, jint);
    JUI_METHOD(void, JUIsetGraphRectNative)(JNIEnv * env, jclass, jint, jint, jint, jint, jint);
}


#endif

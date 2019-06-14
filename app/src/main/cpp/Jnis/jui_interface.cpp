#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <AndroidUtils/AndroidHelper.h>
#include "jui_interface.h"

JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv * env, jclass,jfloat x, jfloat y){
//    renderNative(renderAddr)->onSingleTouchDown(x, y);
}
JUI_METHOD(void, JUIsetInitialValueBool)(JNIEnv * env, jclass, jstring key, jboolean value){
    LOGE("====SET %s, %d", dvr::jstring2string(env,key).c_str(), value);
}
JUI_METHOD(void, JUIsetInitialValueFloat)(JNIEnv * env, jclass, jstring key, jfloat value){
    LOGE("====SET %s, %f", dvr::jstring2string(env,key).c_str(), value);
}
JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint item , jint id){
//    renderNative(renderAddr)->onJavaUIStatusSet(item, id);
}
JUI_METHOD(void, JUIsetParam)(JNIEnv * env, jclass, jint , jfloat){

}
JUI_METHOD(void, JUIsetSwitches)(JNIEnv * env, jclass, jint , jboolean){

}
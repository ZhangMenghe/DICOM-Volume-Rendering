#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <AndroidUtils/AndroidHelper.h>
#include "jui_interface.h"
#include <vrController.h>
using namespace dvr;

JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass,jfloat x, jfloat y){
    nativeApp(nativeAddr)->onSingleTouchDown(x, y);
}
JUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onTouchMove(x, y);
}

JUI_METHOD(void, JUIsetInitialValueBool)(JNIEnv * env, jclass, jstring key, jboolean value){
    LOGE("====SET %s, %d", dvr::jstring2string(env,key).c_str(), value);
//    vrController::param_bool_map[dvr::jstring2string(env,key)] = value;
}
JUI_METHOD(void, JUIsetInitialValueFloat)(JNIEnv * env, jclass, jstring key, jfloat value){
    LOGE("====SET %s, %f", dvr::jstring2string(env,key).c_str(), value);
//    vrController::param_value_map[dvr::jstring2string(env,key)] = value;
}
JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint item , jint id){
//    vrController::jui_status[item] = id;
}
JUI_METHOD(void, JUIsetParam)(JNIEnv * env, jclass, jint , jfloat){

}
JUI_METHOD(void, JUIsetSwitches)(JNIEnv * env, jclass, jint , jboolean){

}
JUI_METHOD(float, JUIgetFPS)(JNIEnv * env, jclass){
    return 60.0f;
}
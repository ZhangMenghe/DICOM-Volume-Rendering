#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <AndroidUtils/AndroidHelper.h>
#include "jui_interface.h"
#include <vrController.h>
#include "AndroidUtils/perfMonitor.h"
using namespace dvr;
namespace {
    perfMonitor fps_monitor_;
}
JUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass,jfloat x, jfloat y){
    nativeApp(nativeAddr)->onSingleTouchDown(x, y);
}
JUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onTouchMove(x, y);
}

JUI_METHOD(void, JUIsetSwitches)(JNIEnv * env, jclass, jstring key, jboolean value){
    LOGE("====SET %s, %d", dvr::jstring2string(env,key).c_str(), value);
    vrController::param_bool_map[dvr::jstring2string(env,key)] = value;

}
JUI_METHOD(void, JUIsetParam)(JNIEnv * env, jclass, jstring key, jfloat value){
    LOGE("====SET %s, %f", dvr::jstring2string(env,key).c_str(), value);
    vrController::param_value_map[dvr::jstring2string(env,key)] = value;
}
JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint item , jint id){
//    vrController::jui_status[item] = id;
}
JUI_METHOD(float, JUIgetFPS)(JNIEnv *, jclass){
    return fps_monitor_.Update();
}
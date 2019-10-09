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
JUI_METHOD(void, JUIonScale)(JNIEnv *, jclass, jfloat sx, jfloat sy){
    nativeApp(nativeAddr)->onScale(sx, sy);
}
JUI_METHOD(void, JUIonPan)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onPan(x,y);
}


JUI_METHOD(void, JUIsetSwitches)(JNIEnv * env, jclass, jstring key, jboolean value){
    LOGE("====SET %s, %d", dvr::jstring2string(env,key).c_str(), value);
    vrController::param_bool_map[dvr::jstring2string(env,key)] = value;


}
JUI_METHOD(void, JUIsetParam)(JNIEnv * env, jclass, jstring key, jfloat value){
//    LOGE("====SET %s, %f", dvr::jstring2string(env,key).c_str(), value);
    vrController::param_value_map[dvr::jstring2string(env,key)] = value;
    if(dvr::jstring2string(env,key) == "cutting")
        vrController::cutDirty = true;
}
JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint item, jstring key){
    if(dvr::jstring2string(env,key) == "Opacity")
        vrController::param_bool_map["Opacity"] = true;
    else
        vrController::param_bool_map["Opacity"] = false;
//    LOGE("====statis: %d",  vrController::param_bool_map["Opacity"]);
}
JUI_METHOD(float, JUIgetFPS)(JNIEnv *, jclass){
    return fps_monitor_.Update();
}
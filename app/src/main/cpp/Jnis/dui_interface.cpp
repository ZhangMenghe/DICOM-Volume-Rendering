#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <AndroidUtils/AndroidHelper.h>
#include "dui_interface.h"
#include <vrController.h>
#include "AndroidUtils/perfMonitor.h"
using namespace dvr;

namespace {
    std::vector<std::string> param_tex_names, param_ray_names, param_checks;
    const int TEX_ID = 0, RAY_ID = 1;
}

DUI_METHOD(void, JUIInitTuneParam)(JNIEnv *env, jclass, jint id, jint num, jobjectArray jkeys, jfloatArray jvalues){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    auto tvec = (id==TEX_ID)? &vrController::param_tex : &vrController::param_ray;
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        jstring jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        std::string key = dvr::jstring2string(env,jkey);
        vec->push_back(key);
        tvec->push_back(values[i]);
        LOGE("======SET INIT %s, %f", key.c_str(), values[i]);
    }
    vrController::baked_dirty_ = true;
}
DUI_METHOD(void, JUIInitCheckParam)(JNIEnv * env, jclass, jint num, jobjectArray jkeys, jbooleanArray jvalues){
    jboolean* values = env->GetBooleanArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        jstring jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        std::string key = dvr::jstring2string(env,jkey);
        param_checks.push_back(key);
        vrController::param_bool.push_back(values[i]);
        LOGE("======SET INIT %s, %d", key.c_str(), values[i]);
    }
    vrController::baked_dirty_ = true;
}

DUI_METHOD(void, JUIsetTuneParam)(JNIEnv *env, jclass, jint id, jstring jkey, jfloat value){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    std::string key = dvr::jstring2string(env,jkey);
    auto it = std::find (vec->begin(), vec->end(), key);
    if (it != vec->end()){
        (id == TEX_ID)? vrController::param_tex[it - vec->begin()] = value : vrController::param_ray[it-vec->begin()] = value;
        LOGE("======SET %d, %s, %f", id, key.c_str(), value);
    }else{
        LOGE("======not find %s", key.c_str());
        vec->push_back(key);
        (id == TEX_ID)? vrController::param_tex.push_back(value) : vrController::param_ray.push_back(value);
    }
    vrController::baked_dirty_ = true;
}
DUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring jkey, jboolean value){
    std::string key = dvr::jstring2string(env,jkey);

    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        vrController::param_bool[it -param_checks.begin()] = value;
        vrController::baked_dirty_ = true;
        LOGE("======SET  %s, %d", key.c_str(), value);
    }else{
        LOGE("======not find %s", key.c_str());
    }
}


//namespace {
//    perfMonitor fps_monitor_;
//}
DUI_METHOD(void, JUIonSingleTouchDown)(JNIEnv *, jclass,jfloat x, jfloat y){
    nativeApp(nativeAddr)->onSingleTouchDown(x, y);
}
DUI_METHOD(void, JUIonTouchMove)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onTouchMove(x, y);
}
DUI_METHOD(void, JUIonScale)(JNIEnv *, jclass, jfloat sx, jfloat sy){
    nativeApp(nativeAddr)->onScale(sx, sy);
}
DUI_METHOD(void, JUIonPan)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onPan(x,y);
}



//JUI_METHOD(void, JUIsetJavaUIStatus)(JNIEnv * env, jclass, jint item, jstring key){
//    if(dvr::jstring2string(env,key) == "Opacity")
//        vrController::param_bool_map["Opacity"] = true;
//    else
//        vrController::param_bool_map["Opacity"] = false;
////    LOGE("====statis: %d",  vrController::param_bool_map["Opacity"]);
//}
//JUI_METHOD(float, JUIgetFPS)(JNIEnv *, jclass){
//    return fps_monitor_.Update();
//}
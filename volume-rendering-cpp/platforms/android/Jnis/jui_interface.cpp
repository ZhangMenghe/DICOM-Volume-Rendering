#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jui_interface.h"
#include <vrController.h>
#include <overlayController.h>

using namespace dvr;

namespace {
    std::vector<std::string> param_checks;
}

JUI_METHOD(void, JUIAddTuneParams)(JNIEnv * env, jclass, jint num, jfloatArray jvalues){
    if(num != dvr::TUNE_END) return;
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    overlayController::instance()->addWidget(std::vector<float>(values, values+num));
}
void InitCheckParam(JNIEnv * env, jint num, jobjectArray jkeys, jbooleanArray jvalues){
    param_checks.clear();
    vrController::param_bool.clear();
    jboolean* values = env->GetBooleanArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        jstring jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        std::string key = dvr::jstring2string(env,jkey);
        param_checks.push_back(key);
        vrController::param_bool.push_back(values[i]);
//        LOGE("======SET INIT %s, %d", key.c_str(), values[i]);
    }
    vrController::baked_dirty_ = true;
}

JUI_METHOD(void, JUIsetTuneWidgetById)(JNIEnv *, jclass, jint wid){
    overlayController::instance()->setWidgetId(wid);
}
JUI_METHOD(void, JUIremoveTuneWidgetById)(JNIEnv *, jclass, jint wid){
    overlayController::instance()->removeWidget(wid);
}
JUI_METHOD(void, JUIremoveAllTuneWidget)(JNIEnv *, jclass){
    overlayController::instance()->removeAll();
}
JUI_METHOD(void, JUIsetTuneParamById)(JNIEnv *, jclass, jint pid, jfloat value){
    if(pid <= dvr::TUNE_END)overlayController::instance()->setTuneParameter(pid, value);
}
JUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring jkey, jboolean value){
    std::string key = dvr::jstring2string(env,jkey);

    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        vrController::param_bool[it -param_checks.begin()] = value;
//        LOGE("======SET  %s, %d", key.c_str(), value);
//        if(key=="Raycasting") vrController::widget_id = value?1:0;
//        if(key==freeze_keyworkd) vrController::cutDirty = true;
//            //!!debug only,
////        else if(key == "Raycasting") vrController::instance()->setMVPStatus(value?"Raycasting":"texturebased");
        vrController::baked_dirty_ = true;
    }
}

JUI_METHOD(void, JUIsetCuttingPlane)(JNIEnv *, jclass, jint id, jfloat value){
//    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
//    auto tvec = (id==TEX_ID)? &vrController::param_tex : &vrController::param_ray;
////    LOGE("======CUTTING %d, %f, %d", id, value, freeze_plane?1:0);
//    auto it = std::find (vec->begin(), vec->end(), cutting_keyword);
//    (*tvec)[it - vec->begin()] = value;
//    vrController::cutDirty = true;
//    vrController::baked_dirty_ = true;
    vrController::instance()->setCuttingPlane(value);
}
JUI_METHOD(void, JUIsetMaskBits)(JNIEnv * env, jclass, jint num, jint mbits){
    vrController::instance()->mask_num_ = (unsigned int)num;
    vrController::instance()->mask_bits_ = (unsigned int)mbits;
    vrController::baked_dirty_ = true;
}
JUI_METHOD(void, JuisetColorScheme)(JNIEnv * env, jclass, jint id){
    vrController::color_scheme_id = id;
    vrController::baked_dirty_ = true;
}
JUI_METHOD(void, JuisetGraphRect)(JNIEnv * env, jclass, jint id, jint width, jint height, jint left, jint top){
    overlayController::instance()->setOverlayRect(id, width, height, left, top);
}
JUI_METHOD(void, JUIonReset)(JNIEnv* env, jclass, jint num, jobjectArray jkeys, jbooleanArray jvalues){
    InitCheckParam(env, num, jkeys, jvalues);
    nativeApp(nativeAddr)->onReset();
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

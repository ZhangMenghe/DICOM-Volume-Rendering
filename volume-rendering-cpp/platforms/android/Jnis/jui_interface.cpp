#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jui_interface.h"
#include <vrController.h>
#include <platforms/android/ARHelpers/arController.h>

using namespace dvr;

namespace {
    std::vector<std::string> param_tex_names, param_ray_names, param_checks;
    const int TEX_ID = 0, RAY_ID = 1;
    //todo:currently should manully keep consistence with R.string....
    const std::string cutting_keyword = "Cutting", freeze_keyworkd="Freeze Plane", ar_keyword = "AR Enable";

}

JUI_METHOD(void, JUIInitTuneParam)(JNIEnv *env, jclass, jint id, jint num, jobjectArray jkeys, jfloatArray jvalues){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    auto tvec = (id==TEX_ID)? &vrController::param_tex : &vrController::param_ray;
    vec->clear();tvec->clear();
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        jstring jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        std::string key = dvr::jstring2string(env,jkey);
        vec->push_back(key);
        tvec->push_back(values[i]);
//        LOGE("======SET INIT %s, %f", key.c_str(), values[i]);
    }
    vec->push_back(cutting_keyword);
    tvec->push_back(-1.0f);

    vrController::baked_dirty_ = true;
}
JUI_METHOD(void, JUIInitCheckParam)(JNIEnv * env, jclass, jint num, jobjectArray jkeys, jbooleanArray jvalues){
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

    param_checks.push_back(freeze_keyworkd);
    vrController::param_bool.push_back(false);

    vrController::baked_dirty_ = true;
}

JUI_METHOD(void, JUIsetTuneParam)(JNIEnv *env, jclass, jint id, jstring jkey, jfloat value){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    std::string key = dvr::jstring2string(env,jkey);
    auto it = std::find (vec->begin(), vec->end(), key);
    if (it != vec->end()){
        (id == TEX_ID)? vrController::param_tex[it - vec->begin()] = value : vrController::param_ray[it-vec->begin()] = value;
//        LOGE("======SET %d, %s, %f", id, key.c_str(), value);
        vrController::baked_dirty_ = true;
    }
}
JUI_METHOD(void, JUIsetChecks)(JNIEnv * env, jclass, jstring jkey, jboolean value){
    std::string key = dvr::jstring2string(env, jkey);

    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        vrController::param_bool[it - param_checks.begin()] = value;
        std::string keystr = jstring2string(env, jkey);

        if(keystr == freeze_keyworkd) vrController::cutDirty = true;
        else if(keystr == ar_keyword){
//            vrController::camera = value? &arCam : &virtualCam; //vrController::instance()->camera->Reset();
            if(value){
                vrController::camera = &arCam;
//                vrController::getMMS(virtualMMS);
//                vrController::setMMS(arMMS);
            }else{
                vrController::camera = &virtualCam;
//                vrController::getMMS(arMMS);
//                vrController::setMMS(virtualMMS);
            }
        }
        vrController::baked_dirty_ = true;
    }

}
JUI_METHOD(void, JUIsetCuttingPlane)(JNIEnv *, jclass, jint id, jfloat value, jboolean freeze_plane){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    auto tvec = (id==TEX_ID)? &vrController::param_tex : &vrController::param_ray;
//    LOGE("======CUTTING %d, %f, %d", id, value, freeze_plane?1:0);
    auto it = std::find (vec->begin(), vec->end(), cutting_keyword);
    (*tvec)[it - vec->begin()] = value;
    if(id == RAY_ID){
        auto it_freeze = std::find (param_checks.begin(), param_checks.end(), freeze_keyworkd);
        vrController::param_bool[it_freeze-param_checks.begin()] = freeze_plane;
    }
    vrController::cutDirty = true;
    vrController::baked_dirty_ = true;
}
JUI_METHOD(void, JUIsetMaskBits)(JNIEnv * env, jclass, jint num, jint mbits){
    vrController::instance()->mask_num_ = (unsigned int)num;
    vrController::instance()->mask_bits_ = (unsigned int)mbits;
    vrController::baked_dirty_ = true;
}


JUI_METHOD(void, JUIonReset)(JNIEnv* env, jclass){
    nativeApp(nativeAddr)->onReset();
}

JUI_METHOD(void ,JUIonDoubleTouchDown)(JNIEnv *, jclass, jfloat x, jfloat y){
    if(vrController::param_bool[dvr::CHECK_ARENABLED])
        arController::instance()->onSingleTouchDown(x,y);
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

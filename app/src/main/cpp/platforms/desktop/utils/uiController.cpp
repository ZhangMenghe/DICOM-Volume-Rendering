#include "uiController.h"
#include <vrController.h>
#include <algorithm>
void uiController::InitTuneParam(){
    const char* ray_keys[3] ={
        "Density",
        "Contrast",
        "Bright"
    };
    float ray_values[3] = {
        400.0f,
        1.0f,
        600.0f
    };
    InitTuneParam(RAY_ID, 3, ray_keys, ray_values);
    const char* tex_keys[3] ={
        "Overall",
        "Lowest",
        "Cut-off"
    };
    float tex_values[3] = {
        1.0f,
        1.0f,
        .0f
    };
    InitTuneParam(TEX_ID, 3, tex_keys, tex_values);


}
void uiController::InitTuneParam(int id, int num, const char*keys[], float values[]){
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    auto tvec = (id==TEX_ID)? &vrController::param_tex : &vrController::param_ray;
    vec->clear();tvec->clear();
    for(int i=0; i<num; i++){
        vec->push_back(std::string(keys[i]));
        tvec->push_back(values[i]);
        LOGE("======SET INIT %s, %f", keys[i], values[i]);
    }
    vec->push_back(cutting_keyword);
    tvec->push_back(-1.0f);

    vrController::baked_dirty_ = true;
}

void uiController::InitCheckParam(){
    const char* keys[5] = {
        "seeVolume",
        "Color Transfer",
        "Cutting",
        "MaskOn",
        "Freeze Volume"
    };
    bool values[5] = {
        true,
        false,
        false,
        false,
        false
    };
    InitCheckParam(5, keys, values);

}
void uiController::InitCheckParam(int num, const char*keys[], bool values[]){
    param_checks.clear();
    vrController::param_bool.clear();
    for(int i=0; i<num; i++){
        param_checks.push_back(std::string(keys[i]));
        vrController::param_bool.push_back(values[i]);
       LOGE("======SET INIT %s, %d", keys[i], values[i]);
    }

    param_checks.push_back(freeze_keyworkd);
    vrController::param_bool.push_back(false);

    vrController::baked_dirty_ = true;
}

void uiController::setTuneParam(int id, const char* key, float value){
    std::string key_str(key);
    auto vec = (id==TEX_ID)? &param_tex_names: &param_ray_names;
    auto it = std::find (vec->begin(), vec->end(), key_str);
    if (it != vec->end()){
        (id == TEX_ID)? vrController::param_tex[it - vec->begin()] = value : vrController::param_ray[it-vec->begin()] = value;
    //    LOGE("======SET %d, %s, %f", id, key.c_str(), value);
        vrController::baked_dirty_ = true;
    }

}
void uiController::setCheck(const char*key, bool value){
    std::string key_str(key);
    auto it = std::find (param_checks.begin(), param_checks.end(), key_str);
    if (it != param_checks.end()){
        vrController::param_bool[it -param_checks.begin()] = value;
//        LOGE("======SET  %s, %d", key.c_str(), value);
        if(key_str==freeze_keyworkd) vrController::cutDirty = true;
        vrController::baked_dirty_ = true;
    }
}
void uiController::setCuttingPlane(int id, float value, bool freeze_plane){

}
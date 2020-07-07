#include "uiController.h"
#include <vrController.h>
#include <overlayController.h>
#include <algorithm>

void uiController::InitAll(){
    // AddTuneParams();
    InitAllTuneParam();
    InitCheckParam();
    setMaskBits(4,30);

    vrController::instance()->onReset(
        glm::vec3(.0f),
        glm::vec3(1.0f),
        glm::mat4(1.0),
        new Camera(
                glm::vec3(.0f,.0f,3.0f),
                glm::vec3(.0,1.0f,.0f),
                glm::vec3(.0f,.0f,2.0f)
        ));
}

void uiController::AddTuneParams(){
    float opa_values[5] = {
        1.0f,
        .0f,
        1.0f,
        2.0f,
        1.0f
    };
    overlayController::instance()->addWidget(std::vector<float>(opa_values, opa_values+5));
    overlayController::instance()->setWidgetId(0);
}

void uiController::InitAllTuneParam(){
    float contrast_values[3] = {
        .0f,
        .3f,
        0.5f
    };
    vrController::instance()->setRenderParam(contrast_values);
}

void uiController::InitCheckParam(){
    const char* keys[7] = {
        "Raycasting",
        "Overlays",
        "Cutting",
        "Freeze Volume",
        "Freeze Plane",
        "Show",
        "Recolor"
    };
    bool values[7] = {
        false,
        true,
        false,
        false,
        false,
        true,
        true
    };
    InitCheckParam(7, keys, values);
}

void uiController::InitCheckParam(int num, const char*keys[], bool values[]){
    param_checks.clear();
    Manager::param_bool.clear();
    for(int i=0; i<num; i++){
        param_checks.push_back(std::string(keys[i]));
        Manager::param_bool.push_back(values[i]);
    //    LOGE("======SET INIT %s, %d\n", keys[i], values[i]);
    }
    Manager::baked_dirty_ = true;
}

void uiController::setMaskBits(int num, unsigned int mbits){
    vrController::instance()->mask_num_ = (unsigned int)num;
    vrController::instance()->mask_bits_ = (unsigned int)mbits;
    Manager::baked_dirty_ = true;
}
void uiController::setMaskBits(helmsley::MaskMsg msg){
    setMaskBits(msg.num(), (unsigned int)msg.mbits());
}
void uiController::setCheck(std::string key, bool value){
    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        Manager::param_bool[it - param_checks.begin()] = value;
        Manager::baked_dirty_ = true;
    }
}
void uiController::setCheck(helmsley::CheckMsg msg){
    setCheck(msg.key(), msg.value());
}
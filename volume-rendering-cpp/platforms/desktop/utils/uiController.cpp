#include "uiController.h"
#include <vrController.h>
#include <overlayController.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

void uiController::InitAll(){
    // AddTuneParams();
    InitAllTuneParam();
    InitCheckParam();
    setMaskBits(7,8+32);

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
        0.1f,
        .0f,
        2.0f,
        0.0f,
        1.0f
    };
    overlayController::instance()->addWidget(std::vector<float>(opa_values, opa_values+5));
    overlayController::instance()->setWidgetId(0);
}

void uiController::InitAllTuneParam(){
    float contrast_values[3] = {
        .0f,
        .8f,
        1.0f
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
        true,
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
#ifdef RPC_ENABLED
void uiController::setMaskBits(helmsley::MaskMsg msg){
    setMaskBits(msg.num(), (unsigned int)msg.mbits());
}

void uiController::setCheck(helmsley::CheckMsg msg){
    setCheck(msg.key(), msg.value());
}
void uiController::onReset(helmsley::ResetMsg msg){
    auto cvs = msg.check_values();
    int num = cvs.size();

    if(param_checks.empty()){
        auto f = msg.check_keys();
        param_checks = std::vector<std::string> (f.begin(), f.end());
    }
    
    Manager::param_bool = std::vector<bool> (cvs.begin(), cvs.end());
    Manager::baked_dirty_ = true;

    auto vps = msg.volume_pose();
    auto cps = msg.camera_pose();
    std::cout<<"size: "<<vps.size()<<" "<<cps.size()<<std::endl;

    vrController::instance()->onReset(
        glm::vec3(vps[0], vps[1], vps[2]),
        glm::vec3(vps[3], vps[4], vps[5]),
        glm::make_mat4(vps.Mutable(6)),
        new Camera(
                glm::vec3(cps[0], cps[1], cps[2]),
                glm::vec3(cps[3], cps[4], cps[5]),
                glm::vec3(cps[6], cps[7], cps[8])
        ));
}
#endif
void uiController::setCheck(std::string key, bool value){
    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        Manager::param_bool[it - param_checks.begin()] = value;
        Manager::baked_dirty_ = true;
    }
}
void uiController::addTuneParams(std::vector<float> values){
    overlayController::instance()->addWidget(values);
}
void uiController::removeTuneWidgetById(int wid){
    overlayController::instance()->removeWidget(wid);
}
void uiController::removeAllTuneWidget(){
    overlayController::instance()->removeAll();
}
void uiController::setTuneParamById(int tid, int pid, float value){
    if(tid == 0 && pid < dvr::TUNE_END)overlayController::instance()->setTuneParameter(pid, value);
    else if(tid == 1) vrController::instance()->setRenderParam(pid, value);
}
void uiController::setAllTuneParamById(int id, std::vector<float> values){  
    if(id == 1)vrController::instance()->setRenderParam(&values[0]);
    else if(id == 2)vrController::instance()->setCuttingPlane(glm::vec3(values[0], values[1], values[2]), glm::vec3(values[3], values[4],values[5]));
}
void uiController::setTuneWidgetVisibility(int wid, bool visibility){
    overlayController::instance()->setWidgetsVisibility(wid, visibility);
    Manager::baked_dirty_ = true;
}
void uiController::setTuneWidgetById(int id){
    overlayController::instance()->setWidgetId(id);
}
void uiController::setCuttingPlane(int id, float value){
    vrController::instance()->setCuttingPlane(value);
}
void uiController::setColorScheme(int id){
    Manager::color_scheme_id = id;
    Manager::baked_dirty_ = true;
}

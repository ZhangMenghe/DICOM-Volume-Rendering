#include <dicomRenderer/Constants.h>
#include "Manager.h"
Camera* Manager::camera = nullptr;
std::vector<bool> Manager::param_bool;
std::vector<std::string> Manager::shader_contents;
bool Manager::baked_dirty_;
bool Manager::new_data_available;
int Manager::color_scheme_id;
int Manager::screen_w, Manager::screen_h;


Manager::Manager(){
    shader_contents = std::vector<std::string>(dvr::SHADER_END);
    screen_w = 0; screen_h = 0;
    onReset();
}
Manager::~Manager(){
    if(camera) delete camera;
    param_bool.clear();
    shader_contents.clear();
}

void Manager::onReset(){
    if(camera){delete camera; camera= nullptr;}
    baked_dirty_ = true;
}
void Manager::onViewChange(int w, int h){
    camera->setProjMat(w, h);
    screen_w = w; screen_h = h;
}
bool Manager::IsCuttingNeedUpdate(){
    return param_bool[dvr::CHECK_CUTTING] || param_bool[dvr::CHECK_CENTER_LINE_TRAVEL];
}
bool Manager::IsCuttingEnabled(){
    return param_bool[dvr::CHECK_CUTTING] ||(param_bool[dvr::CHECK_CENTER_LINE_TRAVEL] && param_bool[dvr::CHECK_TRAVERSAL_VIEW]);
}

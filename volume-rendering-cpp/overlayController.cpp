#include <dicomRenderer/colorbarRenderer.h>
#include <dicomRenderer/graphRenderer.h>
#include "overlayController.h"
#include "vrController.h"

using namespace dvr;
overlayController* overlayController::myPtr_ = nullptr;
overlayController* overlayController::instance(){
    if(myPtr_== nullptr) myPtr_ = new overlayController;
    return myPtr_;
}
overlayController::overlayController(){
    onReset();
    default_widget_points_= nullptr;
    myPtr_ = this;
}
overlayController::~overlayController(){
    for(auto olr:renderers_) delete olr.second;
    renderers_.clear();
    for(auto param:widget_params_) param.clear();
    widget_params_.clear();
    for(auto points:widget_points_) delete[]points;
    widget_points_.clear();
    if(default_widget_points_){delete[]default_widget_points_; default_widget_points_= nullptr;}
}
void overlayController::onReset(){
    overlay_rect_set = false;
    dirty_wid = -1;
    _screen_w = 0; _screen_h = 0;
}
void overlayController::onViewCreated(){
    renderers_[OVERLAY_GRAPH] = new GraphRenderer(vrController::shader_contents[SHADER_OPA_VIZ_VERT], vrController::shader_contents[SHADER_OPA_VIZ_FRAG]);
    renderers_[OVERLAY_COLOR_BARS] = new ColorbarRenderer(vrController::shader_contents[SHADER_COLOR_VIZ_VERT], vrController::shader_contents[SHADER_COLOR_VIZ_FRAG]);
    vrController::shader_contents[SHADER_OPA_VIZ_VERT] = "";vrController::shader_contents[SHADER_OPA_VIZ_FRAG]="";
    vrController::shader_contents[SHADER_COLOR_VIZ_VERT] = "";vrController::shader_contents[SHADER_COLOR_VIZ_FRAG]="";
}
void overlayController::onViewChange(int width, int height){
    if(!rects_.empty()){
        //setup overlay rect
        for(auto &rect:rects_){
            auto& r=rect.second;
            if(r.width > 1.0f){
                r.width/=width; r.left/=width;
                r.height/=height; r.top/=height;
            }else if(_screen_w!=0){
                r.width*=_screen_w/width; r.left*=_screen_w/width;
                r.height*=_screen_h/height; r.top*=_screen_h/height;
            }
            renderers_[rect.first]->setRelativeRenderRect(r.width, r.height, r.left, r.top-r.height);
        }
    }
    _screen_w = width; _screen_h = height;
}
void overlayController::onDraw(){
    if(!overlay_rect_set
    ||!vrController::param_bool[dvr::CHECK_OVERLAY]
    || !vrController::instance()->isDrawing()) return;
    if(dirty_wid >= 0){
        renderers_[dvr::OVERLAY_GRAPH]->setData(widget_points_[widget_id], widget_id);
        renderers_[dvr::OVERLAY_COLOR_BARS]->setUniform("u_widget_num", widget_points_.size());
        renderers_[dvr::OVERLAY_COLOR_BARS]->setUniform("u_opacity", 6*widget_points_.size(), u_opacity_data_);
        dirty_wid = -1;
    }
    for(auto render:renderers_) render.second->Draw();
}
void overlayController::update_widget_points_1d_array(){
    if(u_opacity_data_){delete[]u_opacity_data_; u_opacity_data_= nullptr;}
    u_opacity_data_ = new float[12 * widget_points_.size()];
    for(int i=0;i<widget_points_.size();i++)memcpy(u_opacity_data_+12*i, widget_points_[i], 12* sizeof(float));
}

void overlayController::setWidgetId(int id){
    if(id>=widget_points_.size()) return;
    widget_id = id;
    vrController::baked_dirty_ = true;
}
void overlayController::addWidget(std::vector<float> values){
    widget_params_.push_back(std::vector<float>(dvr::TUNE_END, 0));
    widget_points_.push_back(new float[12]);
    while(values.size() < dvr::TUNE_END) values.push_back(.0f);
    int wid = widget_points_.size()-1;
    memcpy(widget_params_[wid].data(), values.data(), dvr::TUNE_END * sizeof(float));
    if(!default_widget_points_) GraphRenderer::getGraphPoints(widget_params_[wid].data(), default_widget_points_);
    memcpy(widget_points_[wid], default_widget_points_, 12* sizeof(float));
    dirty_wid = wid;
    update_widget_points_1d_array();
    vrController::baked_dirty_ = true;
}
void overlayController::removeWidget(int wid){
    if(wid>=widget_points_.size()) return;

    widget_params_.erase(widget_params_.begin()+wid);
    widget_points_.erase(widget_points_.begin() + wid);
    ((GraphRenderer*)renderers_[dvr::OVERLAY_GRAPH])->removeInstance(wid);
    vrController::baked_dirty_ = true;
}
void overlayController::removeAll(){
    for(auto param:widget_params_) param.clear();
    for(auto param:widget_points_) delete[]param;
    widget_params_.clear();
    widget_points_.clear();
    renderers_[dvr::OVERLAY_GRAPH]->Clear();
    vrController::baked_dirty_ = true;
}
void overlayController::setTuneParameter(int tid, float value){
    if(widget_id>=widget_params_.size() || tid>=dvr::TUNE_END) return;
    widget_params_[widget_id][tid] = value;
    GraphRenderer::getGraphPoints(widget_params_[widget_id].data(), widget_points_[widget_id]);
    dirty_wid = widget_id;
    update_widget_points_1d_array();
    vrController::baked_dirty_ = true;
}
void overlayController::setOverlayRect(int id, int width, int height, int left, int top){
    if(id == 0) _screen_h_offset = top + height;
    top = _screen_h_offset - top;
    if(_screen_w != 0){
        dvr::Rect r{width/_screen_w, height/_screen_h, left/_screen_w, top/_screen_h};
        rects_[(dvr::DRAW_OVERLAY_IDS)id] = r;
        renderers_[(dvr::DRAW_OVERLAY_IDS)id]->setRelativeRenderRect(r.width, r.height, r.left, r.top-r.height);
    }else{
        dvr::Rect r{(float)width, (float)height, (float)left, (float)top};
        rects_[(dvr::DRAW_OVERLAY_IDS)id] = r;
    }
    overlay_rect_set = true;
}
void overlayController::updateUniforms(){
    renderers_[dvr::OVERLAY_COLOR_BARS]->setUniform("uScheme", vrController::color_scheme_id);
}


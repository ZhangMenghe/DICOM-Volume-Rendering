#include <dicomRenderer/Constants.h>
#include <dicomRenderer/graphRenderer.h>
#include "Manager.h"

Camera* Manager::camera = nullptr;
std::vector<bool> Manager::param_bool;
std::vector<std::string> Manager::shader_contents;
bool Manager::baked_dirty_;
bool Manager::new_data_available;
int Manager::color_scheme_id;
dvr::ORGAN_IDS Manager::traversal_target_id;
int Manager::screen_w, Manager::screen_h;
bool Manager::show_ar_ray, Manager::volume_ar_hold;
Manager *Manager::myPtr_ = nullptr;

Manager *Manager::instance()
{
    if (myPtr_ == nullptr)
        myPtr_ = new Manager;
    return myPtr_;
}

Manager::Manager(){
    shader_contents = std::vector<std::string>(dvr::SHADER_ALL_END-1);
    screen_w = 0; screen_h = 0;
    show_ar_ray = false;volume_ar_hold = false;
    onReset();
}
Manager::~Manager(){
    if(camera) delete camera;
    param_bool.clear();
    shader_contents.clear();
}

void Manager::onReset(){
    if(camera){delete camera; camera= nullptr;}
//    clear_opacity_widgets();
    baked_dirty_ = true;
    m_dirty_wid = -1;
}
void Manager::onViewChange(int w, int h){
    camera->setProjMat(w, h);
    screen_w = w; screen_h = h;
}
void Manager::clear_opacity_widgets(){
    m_volset_data.u_visible_bits = 0;
    m_volset_data.u_widget_num = 0;
    for (auto param : widget_params_)
        param.clear();
    widget_params_.clear();
    widget_visibilities_.clear();
    if (default_widget_points_ != nullptr)
    {
        delete[] default_widget_points_;
        default_widget_points_ = nullptr;
    }
    baked_dirty_ = true;
}
void Manager::InitCheckParams(std::vector<std::string> keys, std::vector<bool> values){
    if (keys.size() != values.size())
        return;
    param_checks = keys;
    param_bool = values;

    m_volset_data.u_show_organ = param_bool[dvr::CHECK_MASKON];
    m_volset_data.u_mask_recolor = param_bool[dvr::CHECK_MASK_RECOLOR];

    baked_dirty_ = true;
}
void Manager::addOpacityWidget(float *values, int value_num){
    widget_params_.push_back(std::vector<float>(dvr::TUNE_END, 0));
    widget_visibilities_.push_back(true);

    int wid = m_volset_data.u_widget_num;
    if (value_num < dvr::TUNE_END)
        memset(widget_params_[wid].data(), .0f, dvr::TUNE_END * sizeof(float));
    memcpy(widget_params_[wid].data(), values, value_num * sizeof(float));
    if (!default_widget_points_)
        GraphRenderer::getGraphPoints(values, default_widget_points_);

    memcpy(&m_volset_data.u_opacity[12 * wid], default_widget_points_, 12* sizeof(float));
    m_volset_data.u_widget_num = wid + 1;
    m_volset_data.u_visible_bits |= 1 << wid;
    baked_dirty_ = true;
    m_dirty_wid = wid;
}
void Manager::removeOpacityWidget(int wid){
    if (wid >= m_volset_data.u_widget_num)
        return;
    widget_params_.erase(widget_params_.begin() + wid);
    widget_visibilities_.erase(widget_visibilities_.begin() + wid);

    //update data for shader
    memcpy(&m_volset_data.u_opacity[wid*12],
            &m_volset_data.u_opacity[(wid+1)*12],
            12* (m_volset_data.u_widget_num - wid-1)*sizeof(float));

    m_volset_data.u_widget_num--;
    m_volset_data.u_visible_bits = 0;
    for (int i = 0; i < m_volset_data.u_widget_num; i++)
        m_volset_data.u_visible_bits |= int(widget_visibilities_[i]) << i;
    baked_dirty_ = true;
    m_dirty_wid = wid;
}
void Manager::removeAllOpacityWidgets()
{
    clear_opacity_widgets();
}
void Manager::setRenderParam(int id, float value)
{
    m_render_params[id] = value;
    Manager::baked_dirty_ = true;
    if (id == dvr::RENDER_CONTRAST_LOW)
        m_volset_data.u_contrast_low = value;
    else if (id == dvr::RENDER_CONTRAST_HIGH)
        m_volset_data.u_contrast_high = value;
    else
        m_volset_data.u_brightness = m_render_params[dvr::RENDER_BRIGHTNESS];
}
void Manager::setRenderParam(float *values)
{
    memcpy(m_render_params, values, dvr::PARAM_RENDER_TUNE_END * sizeof(float));
    m_volset_data.u_contrast_low = m_render_params[dvr::RENDER_CONTRAST_LOW];
    m_volset_data.u_contrast_high = m_render_params[dvr::RENDER_CONTRAST_HIGH];
    m_volset_data.u_brightness = m_render_params[dvr::RENDER_BRIGHTNESS];
    Manager::baked_dirty_ = true;
}
void Manager::setCheck(std::string key, bool value)
{
    auto it = std::find(param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end())
    {
        param_bool[it - param_checks.begin()] = value;
        m_volset_data.u_show_organ = param_bool[dvr::CHECK_MASKON];
        m_volset_data.u_mask_recolor = param_bool[dvr::CHECK_MASK_RECOLOR];
        baked_dirty_ = true;
    }
}
void Manager::setMask(unsigned int num, unsigned int bits){
    m_volset_data.u_organ_num = num;
    m_volset_data.u_maskbits = bits;
    Manager::baked_dirty_ = true;
}
void Manager::setColorScheme(int id)
{
    m_volset_data.u_color_scheme = id;
    baked_dirty_ = true;
}
void Manager::setDimension(glm::vec3 dim){
    m_volset_data.u_tex_size = dim;
    new_data_available = true;
}
void Manager::setOpacityWidgetId(int id)
{
    if (id >= m_volset_data.u_widget_num)
        return;
    m_current_wid = id;
    baked_dirty_ = true;
}
void Manager::setOpacityValue(int pid, float value)
{
    if (pid >= dvr::TUNE_END)
        return;
    m_dirty_wid = m_current_wid;
    widget_params_[m_current_wid][pid] = value;

    GraphRenderer::getGraphPoints(widget_params_[m_current_wid].data(), dirty_widget_points_);

    memcpy(&m_volset_data.u_opacity[12 * m_current_wid], dirty_widget_points_, 12 * sizeof(float));

    baked_dirty_ = true;
}
void Manager::setOpacityWidgetVisibility(int wid, bool visible)
{
    m_dirty_wid = wid;
    widget_visibilities_[wid] = visible;
    if (visible)
        m_volset_data.u_visible_bits |= 1 << wid;
    else
        m_volset_data.u_visible_bits &= ~(1 << wid);
    baked_dirty_ = true;
}

bool Manager::isRayCut(){return param_bool[dvr::CHECK_RAYCAST] && param_bool[dvr::CHECK_CUTTING];}
bool Manager::IsCuttingNeedUpdate(){
    return param_bool[dvr::CHECK_CUTTING] || param_bool[dvr::CHECK_CENTER_LINE_TRAVEL];
}
bool Manager::IsCuttingEnabled(){
    return param_bool[dvr::CHECK_CUTTING] ||(param_bool[dvr::CHECK_CENTER_LINE_TRAVEL] && param_bool[dvr::CHECK_TRAVERSAL_VIEW]);
}
bool Manager::isRayCasting() { return param_bool[dvr::CHECK_RAYCAST]; }
void Manager::setTraversalTargetId(int id){
    traversal_target_id = (id == 0) ? dvr::ORGAN_COLON : dvr::ORGAN_ILEUM;
}
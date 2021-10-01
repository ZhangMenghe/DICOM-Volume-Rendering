#include <dicomRenderer/Constants.h>
#include <dicomRenderer/graphRenderer.h>
#include "Manager.h"
#include <GLPipeline/Primitive.h>
Camera* Manager::camera = nullptr;
std::vector<bool> Manager::param_bool;
volumeSetupConstBuffer Manager::m_volset_data;
std::vector<std::string> Manager::shader_contents, Manager::shader_clahes;
bool Manager::baked_dirty_, Manager::mvp_dirty_;
bool Manager::data_assemble_success;
dvr::ORGAN_IDS Manager::traversal_target_id;
int Manager::screen_w, Manager::screen_h;
bool Manager::show_ar_ray, Manager::volume_ar_hold;
float Manager::indiv_rendering_params[3] = {1.0f, 0.3f, 100.f};
Manager *Manager::myPtr_ = nullptr;

Manager *Manager::instance()
{
    if (myPtr_ == nullptr)
        myPtr_ = new Manager;
    return myPtr_;
}

Manager::Manager(){
    shader_contents = std::vector<std::string>(dvr::SHADER_ALL_END-1);
    shader_clahes = std::vector<std::string>(15);
    screen_w = 0; screen_h = 0;
    show_ar_ray = false;volume_ar_hold = false;
    onReset();
    myPtr_ = this;
}
Manager::~Manager(){
    param_bool.clear();
    shader_contents.clear();
    shader_clahes.clear();

    m_mvp_status.clear();
    camera = nullptr;
}

void Manager::onReset(){
    m_mvp_status.clear();
    m_current_mvp_name.clear(); m_last_mvp_name.clear();

    camera = new Camera;
    clear_opacity_widgets();
    baked_dirty_ = true; mvp_dirty_ = false;
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
void Manager::addOpacityWidget(std::vector<float> values){
    int value_num = values.size();
    widget_params_.emplace_back(std::vector<float>(dvr::TUNE_END, 0));
    widget_visibilities_.push_back(true);

    int wid = m_volset_data.u_widget_num;
    if (value_num < dvr::TUNE_END)
        memset(widget_params_[wid].data(), .0f, dvr::TUNE_END * sizeof(float));
    memcpy(widget_params_[wid].data(), values.data(), value_num * sizeof(float));
    if (!default_widget_points_)
        GraphRenderer::getGraphPoints(values.data(), default_widget_points_);

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
    else if (id==dvr::RENDER_BRIGHTNESS)
        m_volset_data.u_brightness = value;
    else
        m_volset_data.u_base_value = value;
}
void Manager::setRenderParam(float *values)
{
    memcpy(m_render_params, values, dvr::PARAM_RENDER_TUNE_END * sizeof(float));
    m_volset_data.u_contrast_low = m_render_params[dvr::RENDER_CONTRAST_LOW];
    m_volset_data.u_contrast_high = m_render_params[dvr::RENDER_CONTRAST_HIGH];
    m_volset_data.u_brightness = m_render_params[dvr::RENDER_BRIGHTNESS];
    m_volset_data.u_base_value = m_render_params[dvr::RENDER_BASE_VALUE];
    Manager::baked_dirty_ = true;
}

void Manager::setCheck(std::string key, bool value, int& id){
    auto it = std::find(param_checks.begin(), param_checks.end(), key);
    if (it == param_checks.end()) return;
    id = it - param_checks.begin();
    param_bool[id] = value;

    m_volset_data.u_show_organ = param_bool[dvr::CHECK_MASKON];
    m_volset_data.u_mask_recolor = param_bool[dvr::CHECK_MASK_RECOLOR];

    baked_dirty_ = true;
}
void Manager::setMask(unsigned int num, unsigned int bits){
    m_volset_data.u_organ_num = num;
    m_volset_data.u_maskbits = bits;
    Manager::baked_dirty_ = true;
}
void Manager::setColorScheme(int id){
    m_volset_data.u_color_scheme = id;
    baked_dirty_ = true;
}
void Manager::setDimension(glm::vec3 dim){
    m_volset_data.u_tex_size = dim;
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
void Manager::setOpacityWidgetVisibility(int wid, bool visible){
    widget_visibilities_[wid] = visible;
    if (visible)
        m_volset_data.u_visible_bits |= 1 << wid;
    else
        m_volset_data.u_visible_bits &= ~(1 << wid);
    baked_dirty_ = true;
}
void Manager::updateVolumeSetupUniforms(GLuint sp){
    if(m_volset_data.u_color_scheme > 0)
        Shader::Uniform(sp, "u_hex_color_scheme", 256, color_schemes_hex[m_volset_data.u_color_scheme - 1]);

    Shader::Uniform(sp, "u_tex_size", m_volset_data.u_tex_size);
    Shader::Uniform(sp, "u_maskbits", m_volset_data.u_maskbits);
    Shader::Uniform(sp, "u_organ_num", m_volset_data.u_organ_num);
    Shader::Uniform(sp, "u_mask_color", m_volset_data.u_mask_recolor);

    Shader::Uniform(sp, "u_visible_bits", m_volset_data.u_visible_bits);
    Shader::Uniform(sp, "u_opacity", 6*m_volset_data.u_widget_num, 2, m_volset_data.u_opacity);
    Shader::Uniform(sp, "u_widget_num", m_volset_data.u_widget_num);

    Shader::Uniform(sp, "u_contrast_low", m_volset_data.u_contrast_low);
    Shader::Uniform(sp, "u_contrast_high", m_volset_data.u_contrast_high);
    Shader::Uniform(sp, "u_brightness", m_volset_data.u_brightness);
    Shader::Uniform(sp, "u_base_value", m_volset_data.u_base_value);
}
void Manager::addMVPStatus(const std::string& name, glm::mat4 rm, glm::vec3 sv, glm::vec3 pv, bool use_as_current_status){
    auto it = m_mvp_status.find(name);
    if(it == m_mvp_status.end()) m_mvp_status[name] = new reservedStatus(rm, sv, pv);
    else{m_mvp_status[name]->pos_vec = pv; m_mvp_status[name]->rot_mat=rm; m_mvp_status[name]->scale_vec=sv;}
    if(use_as_current_status) setMVPStatus(name);
}

void Manager::addMVPStatus(const std::string& name, bool use_as_current_status){
    auto it = m_mvp_status.find(name);
    if (it != m_mvp_status.end()) delete m_mvp_status[name];
    m_mvp_status[name] = new reservedStatus();
    if(use_as_current_status) setMVPStatus(name);
}
bool Manager::removeMVPStatus(const std::string& name) {
    auto it = m_mvp_status.find(name);
    if (it == m_mvp_status.end()) return false;
    m_mvp_status.erase(name);
    setMVPStatus(m_mvp_status.begin()->first);
}
bool Manager::setMVPStatus(const std::string& name){
    if(m_mvp_status.find(name) == m_mvp_status.end()) return false;
    if(name == m_current_mvp_name) return true;
    m_last_mvp_name = m_current_mvp_name; m_current_mvp_name = name;
    mvp_dirty_ = true;
    return true;
}
void Manager::getCurrentMVPStatus(glm::mat4& rm, glm::vec3& sv, glm::vec3& pv){
    camera->Reset();
    if(screen_w != 0)camera->setProjMat(screen_w, screen_h);

    if(!m_last_mvp_name.empty() && m_mvp_status.find(m_last_mvp_name)!=m_mvp_status.end()){
        m_mvp_status[m_last_mvp_name]->rot_mat = rm; m_mvp_status[m_last_mvp_name]->scale_vec = sv;m_mvp_status[m_last_mvp_name]->pos_vec = pv;
    }
    auto rstate_ = m_mvp_status[m_current_mvp_name];
    rm=rstate_->rot_mat; sv=rstate_->scale_vec; pv=rstate_->pos_vec;

    mvp_dirty_ = false;
}
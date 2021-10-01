#ifndef MANAGER_H
#define MANAGER_H
#include <vector>
#include <GLPipeline/Camera.h>
#include <dicomRenderer/Constants.h>
#include <platforms/platform.h>
#include <unordered_map>
struct volumeSetupConstBuffer{
    glm::vec3 u_tex_size;

    //opacity widget
    float u_opacity[120];
    int u_widget_num;
    int u_visible_bits;

    //contrast
    float u_contrast_low;
    float u_contrast_high;
    float u_brightness;
    float u_base_value;

    //mask
    unsigned int u_maskbits;
    unsigned int u_organ_num;
    bool u_mask_recolor;

    //others
    int u_show_organ;
    int u_color_scheme;
    volumeSetupConstBuffer(){
        u_tex_size = glm::vec3(.0);u_widget_num=0;u_visible_bits=0;
        u_color_scheme = 0;
    }
};

struct reservedStatus{
    glm::mat4 rot_mat;
    glm::vec3 scale_vec, pos_vec;
    reservedStatus(glm::mat4 rm, glm::vec3 sv, glm::vec3 pv){
        rot_mat=rm; scale_vec=sv; pos_vec=pv;
    }
    reservedStatus():rot_mat(dvr::DEFAULT_ROTATE), scale_vec(dvr::DEFAULT_SCALE), pos_vec(dvr::DEFAULT_POS){}
};

class Manager {
public:
    static Manager *instance();

    static Camera* camera;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents, shader_clahes;

    static bool baked_dirty_, mvp_dirty_;
    static dvr::ORGAN_IDS traversal_target_id;
    static int screen_w, screen_h;
    static bool show_ar_ray, volume_ar_hold;
    static bool data_assemble_success;
    static float indiv_rendering_params[3];

    //static getters
    static unsigned int getMaskBits() { return m_volset_data.u_maskbits; }
    static unsigned int getMaskNum() { return m_volset_data.u_organ_num; }
    static volumeSetupConstBuffer *getVolumeSetupConstData() { return &m_volset_data; }

    static bool IsCuttingEnabled(){return param_bool[dvr::CHECK_CUTTING] ||(param_bool[dvr::CHECK_CENTER_LINE_TRAVEL] && param_bool[dvr::CHECK_TRAVERSAL_VIEW]);}
    static bool IsCuttingNeedUpdate(){return param_bool[dvr::CHECK_CUTTING] || param_bool[dvr::CHECK_CENTER_LINE_TRAVEL];}
    static void setTraversalTargetId(int id){traversal_target_id = (id == 0) ? dvr::ORGAN_COLON : dvr::ORGAN_ILEUM;}
    static bool isARWithMarker(){return param_bool[dvr::CHECK_AR_ENABLED] && (!param_bool[dvr::CHECK_AR_USE_ARCORE]);}
    static bool isDrawVolume() { return !param_bool[dvr::CHECK_MASKON] || param_bool[dvr::CHECK_VOLUME_ON]; }
    static bool isDrawCenterLine() { return param_bool[dvr::CHECK_MASKON] && Manager::param_bool[dvr::CHECK_CENTER_LINE]; }
    static bool isDrawMesh() { return param_bool[dvr::CHECK_MASKON] && Manager::param_bool[dvr::CHECK_DRAW_POLYGON]; }

    Manager();
    ~Manager();
    void onReset();
    void onViewChange(int w, int h);
    void InitCheckParams(std::vector<std::string> keys, std::vector<bool> values);

    //Getters
    int getDirtyOpacityId() { return m_dirty_wid; }
    float *getDefaultWidgetPoints() { return default_widget_points_; }
    float *getDirtyWidgetPoints() { return dirty_widget_points_; }
    std::vector<bool>* getOpacityWidgetVisibility(){return &widget_visibilities_;}
    const char* getColorSchemeKeyword(){
        return COLOR_SCHEMES[m_volset_data.u_color_scheme];
    }

    //Adders
    void addOpacityWidget(std::vector<float> values);
    void removeOpacityWidget(int wid);
    void removeAllOpacityWidgets();

    //Setters
    void setRenderParam(int id, float value);
    void setRenderParam(float *values);
    void setCheck(std::string key, bool value, int& id);
    void setMask(unsigned int num, unsigned int bits);
    void setColorScheme(int id);
    void setDimension(glm::vec3 dim);
    void setOpacityWidgetId(int id);
    void setOpacityValue(int pid, float value);
    void setOpacityWidgetVisibility(int wid, bool visible);
    void resetDirtyOpacityId() { m_dirty_wid = -1; }
    void updateVolumeSetupUniforms(GLuint sp);

    //mvp status
    void addMVPStatus(const std::string& name, glm::mat4 rm, glm::vec3 sv, glm::vec3 pv, bool use_as_current_status);
    void addMVPStatus(const std::string& name, bool use_as_current_status);
    bool removeMVPStatus(const std::string& name);
    bool setMVPStatus(const std::string& name);
    void getCurrentMVPStatus(glm::mat4& rm, glm::vec3& sv, glm::vec3& pv);
private:
    static Manager *myPtr_;
    static volumeSetupConstBuffer m_volset_data;
    std::unordered_map<std::string, reservedStatus*> m_mvp_status;

    //contrast, brightness, etc
    float m_render_params[dvr::PARAM_RENDER_TUNE_END] = {.0f};

    //opacity widgets
    std::vector<std::vector<float>> widget_params_;
    std::vector<bool> widget_visibilities_;
    float *default_widget_points_ = nullptr, *dirty_widget_points_ = nullptr;
    int m_current_wid = -1;
    int m_dirty_wid;

    //check names
    std::vector<std::string> param_checks;

    //mvp status
    std::string m_last_mvp_name, m_current_mvp_name;

    //color
    //color scheme
    const char* COLOR_SCHEMES[5]={
            "COLOR_GRAYSCALE",
            "COLOR_HSV",
            "COLOR_BRIGHT",
            "COLOR_FIRE",
            "COLOR_CET_L08",
    };

    //ar status
    void clear_opacity_widgets();
};
#endif //VOLUME_RENDERING_MANAGER_H

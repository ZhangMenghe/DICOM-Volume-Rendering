#ifndef MANAGER_H
#define MANAGER_H
#include <vector>
#include <GLPipeline/Camera.h>
#include <dicomRenderer/Constants.h>
#include <platforms/platform.h>

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

class Manager {
public:
    static Manager *instance();

    static Camera* camera;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents;

    static bool baked_dirty_;
    static dvr::ORGAN_IDS traversal_target_id;
    static int screen_w, screen_h;
    static bool show_ar_ray, volume_ar_hold;
    static bool isRayCut();
    static bool new_data_available;

    Manager();
    ~Manager();
    void onReset();
    void onViewChange(int w, int h);
    void InitCheckParams(std::vector<std::string> keys, std::vector<bool> values);

    static bool IsCuttingEnabled();
    static bool IsCuttingNeedUpdate();
    static bool isRayCasting();
    static void setTraversalTargetId(int id);

    //Getters
    volumeSetupConstBuffer *getVolumeSetupConstData() { return &m_volset_data; }
    unsigned int getMaskBits() { return m_volset_data.u_maskbits; }
    bool getCheck(dvr::PARAM_BOOL id) { return param_bool[id]; }
    bool isDrawVolume() { return !param_bool[dvr::CHECK_MASKON] || param_bool[dvr::CHECK_VOLUME_ON]; }
    bool isDrawCenterLine() { return param_bool[dvr::CHECK_MASKON] && Manager::param_bool[dvr::CHECK_CENTER_LINE]; }
    bool isDrawMesh() { return param_bool[dvr::CHECK_MASKON] && Manager::param_bool[dvr::CHECK_DRAW_POLYGON]; }
    int getDirtyOpacityId() { return m_dirty_wid; }
    float *getDefaultWidgetPoints() { return default_widget_points_; }
    float *getDirtyWidgetPoints() { return dirty_widget_points_; }
    int getColorScheme(){return m_volset_data.u_color_scheme;}
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
    void setCheck(std::string key, bool value);
    void setMask(unsigned int num, unsigned int bits);
    void setColorScheme(int id);
    void setDimension(glm::vec3 dim);
    void setOpacityWidgetId(int id);
    void setOpacityValue(int pid, float value);
    void setOpacityWidgetVisibility(int wid, bool visible);
    void resetDirtyOpacityId() { m_dirty_wid = -1; }
    void updateVolumeSetupUniforms(GLuint sp);

private:
    static Manager *myPtr_;
    volumeSetupConstBuffer m_volset_data;

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

    //color
    //color scheme
    const char* COLOR_SCHEMES[5]={
            "COLOR_GRAYSCALE",
            "COLOR_HSV",
            "COLOR_BRIGHT",
            "COLOR_FIRE",
            "COLOR_CET_L08",
    };

    void clear_opacity_widgets();
};
#endif //VOLUME_RENDERING_MANAGER_H

#ifndef VOLUME_RENDERING_MANAGER_H
#define VOLUME_RENDERING_MANAGER_H

#include "GLPipeline/Camera.h"
#include "dicomRenderer/Constants.h"
#include <vector>

class Manager {
public:
    static Camera* camera;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents;

    static bool baked_dirty_;
    static int color_scheme_id;
    static dvr::ORGAN_IDS traversal_target_id;
    static int screen_w, screen_h;
    static bool show_ar_ray, volume_ar_hold;
    static bool isRayCut();
    static bool new_data_available;

    Manager();
    ~Manager();
    void onReset();
    void onViewChange(int w, int h);
    static bool IsCuttingEnabled();
    static bool IsCuttingNeedUpdate();
};


#endif //VOLUME_RENDERING_MANAGER_H

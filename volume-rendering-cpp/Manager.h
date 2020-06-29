#ifndef VOLUME_RENDERING_MANAGER_H
#define VOLUME_RENDERING_MANAGER_H

#include "GLPipeline/Camera.h"
#include <vector>

class Manager {
public:
    static Camera* camera;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents;

    static bool baked_dirty_;
    static int color_scheme_id;
    static int screen_w, screen_h;
    static bool show_ar_ray, volume_ar_hold;
    static bool isRayCut();

    Manager();
    ~Manager();
    void onReset();
    void onViewChange(int w, int h);
};


#endif //VOLUME_RENDERING_MANAGER_H

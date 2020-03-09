#ifndef PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#define PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#include <platforms/platform.h>
#include <string>
#include <vector>

class uiController{
private:
    std::vector<std::string> param_tex_names, param_ray_names, param_checks;
    const int TEX_ID = 0, RAY_ID = 1;
    const std::string cutting_keyword = "Cutting", freeze_keyworkd="Freeze Plane";
public:
    void InitTuneParam();
    void InitTuneParam(int id, int num, const char*keys[], float values[]);

    void InitCheckParam();
    void InitCheckParam(int num, const char*keys[], bool values[]);

    void setTuneParam(int id, const char* key, float value);
    void setCheck(const char*key, bool value);
    void setCuttingPlane(int id, float value, bool freeze_plane);

};
#endif
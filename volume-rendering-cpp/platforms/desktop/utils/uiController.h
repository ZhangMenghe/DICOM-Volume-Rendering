#ifndef PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#define PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#include <platforms/platform.h>
#include <string>
#include <vector>

class uiController{
private:
    std::vector<std::string> param_checks;
public:
    void AddTuneParams();
    void InitAll();
    void InitAllTuneParam();

    void InitCheckParam();
    void InitCheckParam(int num, const char*keys[], bool values[]);
    void setMaskBits(int num, unsigned int mbits);
};
#endif
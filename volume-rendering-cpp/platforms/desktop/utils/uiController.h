#ifndef PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#define PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#include <platforms/platform.h>
#include <string>
#include <vector>
#include <proto/inspectorSync.grpc.pb.h>
#include <proto/inspectorSync.pb.h>
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
    void setMaskBits(helmsley::MaskMsg msg);
    void setCheck(std::string key, bool value);
    void setCheck(helmsley::CheckMsg msg);

};
#endif
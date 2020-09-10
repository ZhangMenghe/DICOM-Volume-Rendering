#ifndef PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#define PLATFORMS_DESKTOP_UTILS_UICONTROLLER_H
#include <platforms/platform.h>
#include <string>
#include <vector>
#ifdef RPC_ENABLED
#include <proto/inspectorSync.grpc.pb.h>
#include <proto/inspectorSync.pb.h>
#endif
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
    void setCheck(std::string key, bool value);
#ifdef RPC_ENABLED

    void setMaskBits(helmsley::MaskMsg msg);
    void setCheck(helmsley::CheckMsg msg);
    void onReset(helmsley::ResetMsg msg);
#endif

    void addTuneParams(std::vector<float> values);
    void removeTuneWidgetById(int id);
    void removeAllTuneWidget();
    void setTuneParamById(int tid, int pid, float value);
    void setAllTuneParamById(int tid, std::vector<float> values);
    void setTuneWidgetVisibility(int wid, bool visibility);
    void setTuneWidgetById(int id);
    void setCuttingPlane(int id, float value);
    void setColorScheme(int id);
};
#endif
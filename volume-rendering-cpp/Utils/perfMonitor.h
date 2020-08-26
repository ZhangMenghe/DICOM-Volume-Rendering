#ifndef UTILS_PERFMONITOR_H
#define UTILS_PERFMONITOR_H


#include <cstdint>
#include <ctime>
const int32_t kNumSamples = 100;
class perfMonitor {
private:
    float current_FPS_;
    time_t tv_last_sec_;

    double last_tick_;
    int32_t tickindex_;
    double ticksum_;
    double ticklist_[kNumSamples];

    double UpdateTick(double current_tick);

public:
    perfMonitor();
    float Update();
};


#endif //MYGLES_PERFMONITOR_H

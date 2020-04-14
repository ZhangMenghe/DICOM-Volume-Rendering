#ifndef OVERLAY_CONTROLLER_H
#define OVERLAY_CONTROLLER_H

#include <vector>
#include <unordered_map>
#include <dicomRenderer/basequadRenderer.h>
#include <dicomRenderer/Constants.h>

class overlayController{
public:
    static overlayController* instance();
    overlayController();
    ~overlayController();
    void onReset();
    void onViewCreated();
    void onViewChange(int width, int height);
    void onDraw();

    void setWidgetId(int id);
    void addWidget(std::vector<float> values);
    void removeWidget(int wid);
    void removeAll();

    void setTuneParameter(int tid, float value);
    void setOverlayRect(int id, int width, int height, int left, int top);
    void updateUniforms();

    //getter
    const std::vector<float*> getWidgetPoints(){return widget_points_;}
    const float* getCurrentWidgetPoints(){return widget_points_[widget_id];}


private:
    static overlayController* myPtr_;

    //flag
    bool data_dirty, overlay_rect_set;
    std::unordered_map<dvr::DRAW_OVERLAY_IDS, dvr::Rect> rects_;
    std::unordered_map<dvr::DRAW_OVERLAY_IDS, baseQuad*> renderers_;

    //widgets num should be the same
    int widget_id;
    std::vector<std::vector<float>> widget_params_;
    std::vector<float*> widget_points_;
    float* default_widget_points_;

    //rect
    float _screen_w, _screen_h;
    int _screen_h_offset;

};
#endif
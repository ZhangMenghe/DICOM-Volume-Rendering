#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H
#include <vector>
#include <unordered_map>
#include <dicomRenderer/basequadRenderer.h>
#include <dicomRenderer/Constants.h>
#include <Manager.h>

class dataBoard {
public:
    dataBoard();
    ~dataBoard();

    void onViewChange(int width, int height);
    void onUpdate(Manager* manager);
    void onDraw(bool pre_draw);

    //SETTERS
    void setOverlayRect(int id, int width, int height, int left, int top);
private:
    std::unordered_map<dvr::DRAW_OVERLAY_IDS, dvr::Rect> m_rects;
    std::unordered_map<dvr::DRAW_OVERLAY_IDS, baseQuad*> m_renderers;
    int m_opcity_widget_num;

    //flags
    bool m_overlay_rect_set;
    bool baked_dirty_;

    //rects
    float m_screen_w, m_screen_h;
    int m_screen_h_offset;

    GLuint frame_buff_ = 0;
    void draw_scene();
    void draw_baked();
};


#endif

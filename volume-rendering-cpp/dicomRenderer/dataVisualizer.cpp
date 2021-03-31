#include "dataVisualizer.h"
#include "graphRenderer.h"
#include "colorbarRenderer.h"
#include "screenQuad.h"

using namespace dvr;

dataBoard::dataBoard(){
    m_renderers[OVERLAY_GRAPH] = new GraphRenderer(Manager::shader_contents[SHADER_OPA_VIZ_VERT], Manager::shader_contents[SHADER_OPA_VIZ_FRAG]);
    m_renderers[OVERLAY_COLOR_BARS] = new ColorbarRenderer(Manager::shader_contents[SHADER_COLOR_VIZ_VERT], Manager::shader_contents[SHADER_QUAD_FRAG]);
    m_overlay_rect_set = false;
    m_screen_w = 0; m_screen_h = 0;
    m_opcity_widget_num = 0;
}
dataBoard::~dataBoard(){
    m_renderers.clear();
}
void dataBoard::onViewChange(int width, int height){
    if(!m_rects.empty()){
        //setup overlay rect
        for(auto &rect:m_rects){
            auto& r=rect.second;
            if(r.width > 1.0f){
                r.width/=width; r.left/=width;
                r.height/=height; r.top/=height;
            }else if(m_screen_w!=0){
                r.width*=m_screen_w/width; r.left*=m_screen_w/width;
                r.height*=m_screen_h/height; r.top*=m_screen_h/height;
            }
            m_renderers[rect.first]->setRelativeRenderRect(r.width, r.height, r.left, r.top-r.height);
        }
    }
    m_screen_w = width; m_screen_h = height;
}
void dataBoard::onUpdate(Manager* manager){
    volumeSetupConstBuffer* vol_setup = Manager::getVolumeSetupConstData();
    if (vol_setup->u_widget_num == 0) {
        m_renderers[dvr::OVERLAY_GRAPH]->Clear();
    }else{
        int dirty_wid = manager->getDirtyOpacityId();
        if(dirty_wid < 0) return;
        switch(vol_setup->u_widget_num - m_opcity_widget_num){
            //remove graph at dirty wid
            case -1:
                ((GraphRenderer*)m_renderers[dvr::OVERLAY_GRAPH])->removeInstance(dirty_wid);
                m_opcity_widget_num = vol_setup->u_widget_num;
                break;
            case 0:
                ((GraphRenderer*)m_renderers[dvr::OVERLAY_GRAPH])->setData(&vol_setup->u_opacity[12 * dirty_wid], dirty_wid);
                break;
            case 1:
                ((GraphRenderer*)m_renderers[dvr::OVERLAY_GRAPH])->addInstance();
                ((GraphRenderer*)m_renderers[dvr::OVERLAY_GRAPH])->setData(&vol_setup->u_opacity[12 * dirty_wid], dirty_wid);
                m_opcity_widget_num = vol_setup->u_widget_num;
                break;
        }
    }
    manager->resetDirtyOpacityId();
}
void dataBoard::onDraw(bool pre_draw){
    if(!m_overlay_rect_set) return;
    if(pre_draw || Manager::param_bool[dvr::CHECK_AR_ENABLED]) draw_baked();
    else draw_scene();
}
void dataBoard::draw_scene(){
    for(auto render:m_renderers) render.second->Draw();
}
void dataBoard::draw_baked(){
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void dataBoard::setOverlayRect(int id, int width, int height, int left, int top){
    if(id == 0) m_screen_h_offset = top + height;
    top = m_screen_h_offset - top;
    if(m_screen_w != 0){
        dvr::Rect r{width/m_screen_w, height/m_screen_h, left/m_screen_w, top/m_screen_h};
        m_rects[(dvr::DRAW_OVERLAY_IDS)id] = r;
        m_renderers[(dvr::DRAW_OVERLAY_IDS)id]->setRelativeRenderRect(r.width, r.height, r.left, r.top-r.height);
    }else{
        dvr::Rect r{(float)width, (float)height, (float)left, (float)top};
        m_rects[(dvr::DRAW_OVERLAY_IDS)id] = r;
    }
    m_overlay_rect_set = true;
}
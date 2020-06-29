#ifndef VOLUME_RENDERING_LINERENDERER_H
#define VOLUME_RENDERING_LINERENDERER_H

#include <GLPipeline/Shader.h>
#include "PointCloudRenderer.h"

class lineRenderer:public PointCloudRenderer{
public:
    lineRenderer(bool screen_baked = true);
    void Draw(glm::mat4 mvp);
    void setStartPoint(float x, float y);
    glm::vec3 getEndPosWorld(){return far_pos;}
private:
    Shader shader_;
    bool initialized = false;
    glm::vec2 startPoint;
    glm::vec3 far_pos;
    void draw_scene(glm::mat4 mvp);
};
#endif

#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include "baseDicomRenderer.h"
class raycastRenderer : public baseDicomRenderer{
private:
    Shader *cshader_ = nullptr;
    GLuint vao_cube_= 0;
    float m_sample_steps = 100.f;

    void draw_to_texture(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);
    void draw_scene(glm::mat4 model_mat);
public:
    raycastRenderer();
    void Draw(bool pre_draw, glm::mat4 model_mat);
    void setRenderingParameters(float* values){m_sample_steps = values[0];}
};
#endif
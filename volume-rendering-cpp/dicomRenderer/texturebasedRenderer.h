#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H


#include "baseDicomRenderer.h"

class texvrRenderer:public baseDicomRenderer{
private:
    float vol_thickness_factor = 1.0f;
    int dimension_draw;
    float dimension_draw_inv;
    float dense_factor = 1.0f;
    const int MAX_DIMENSIONS = 1000;

    GLuint vao_front = 0, vbo_front;
    GLuint vao_back = 0, vbo_back;

    bool b_init_successful = false;
    int cut_id = 0;

    //for screen baking
    void init_vertices(GLuint &vao_slice, GLuint& vbo_instance,bool is_front);
    void draw_scene(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);
    void update_instance_data(GLuint& vbo_instance,bool is_front);
    void on_update_dimension_draw();
public:
    texvrRenderer();
    void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale);
    void setCuttingPlane(float percent);
    void setCuttingPlaneDelta(int delta);
    void Draw(bool pre_draw, glm::mat4 model_mat);
    void setRenderingParameters(float* values);
};
#endif
#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H

#include <cstring>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class texvrRenderer{
private:
    const float DENSE_FACTOR = 1.0f;
    int dimensions; float dimension_inv;
    float vol_thickness_factor = 1.0f;
    const int MAX_DIMENSIONS = 1000;

    Shader* shader_;
    GLuint frame_buff_ = 0;
    GLuint vao_front = 0, vbo_front;
    GLuint vao_back = 0, vbo_back;

    bool b_init_successful = false;
    int cut_id = 0;

    //for screen baking
    bool baked_dirty_ = true;
    void init_vertices(GLuint &vao_slice, GLuint& vbo_instance,bool is_front);
    void draw_scene();
    void draw_baked();
    void update_instance_data(GLuint& vbo_instance,bool is_front);

public:
    texvrRenderer();
    void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale);
    void setCuttingPlane(float percent);
    void setCuttingPlaneDelta(int delta);
    void Draw(bool pre_draw);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
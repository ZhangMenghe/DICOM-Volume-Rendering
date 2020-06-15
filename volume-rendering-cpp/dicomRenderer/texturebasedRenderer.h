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
    GLuint vao_slice = 0, vbo_instance;
    bool DRAW_BAKED;
    bool b_init_successful = false;
    int cut_id = 0;

    //for screen baking
    bool baked_dirty_ = true;
    void init_vertices();
    void draw_scene();
    void draw_baked();
    void update_instance_data();

public:
    texvrRenderer(bool screen_baked = true);
    void setDimension(int dims, float thickness);
    void setCuttingPlane(float percent);
    float getCuttingPlane(){}
    void Draw();
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
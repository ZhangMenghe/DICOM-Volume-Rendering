#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H

#include <cstring>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class texvrRenderer{
private:
    const float scale_inv = 0.5f;
    const float DENSE_FACTOR = 1.0f;
    int dimensions; float dimension_inv;

    Shader* shader_;
    GLuint frame_buff_ = 0;
    GLuint vao_slice = 0;
    bool DRAW_BAKED;
    bool b_init_successful = false;

    //for screen baking
    bool baked_dirty_ = true;
    void init_vertices();
    void draw_scene();
    void draw_baked();

public:
    texvrRenderer(bool screen_baked = true);
  
    void setDimension(int dims);
    void onCuttingChange(float percent);
    void Draw();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H

#include <cstring>
#include <GLES3/gl32.h>
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

    //for screen baking
    bool baked_dirty_ = true;
    void two_pass_draw();
    void draw_scene();

public:
    texvrRenderer(bool screen_baked = true);
    void onCuttingChange(float percent);
    void Draw();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
#include "cuttingController.h"
class raycastRenderer{
private:
    cuttingController* cutter_;
    Shader* shader_= nullptr, *cshader_ = nullptr;
    GLuint vao_cube_= 0;
    bool DRAW_BAKED;
    bool baked_dirty_ = true;

    void precompute();
    void draw_baked();
public:
    raycastRenderer(bool screen_baked = true);

    void onCuttingChange(float percent);
    void Draw();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
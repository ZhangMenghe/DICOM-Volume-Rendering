#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
#include "cuttingController.h"
class raycastRenderer{
private:
    cuttingController* cutter_;
    Shader* shader_= nullptr, *shader_baked_= nullptr, *cshader_ = nullptr;
    GLuint VAO_=0, vao_screen_ = 0;

    Texture* ray_baked_screen;
    bool baked_dirty_ = true;
    int BAKED_RAY_SCREEN_ID;
    void precompute();
public:
    raycastRenderer();
    void onCuttingChange(float percent);
    void Draw();
    void DrawBaked();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}

};
#endif
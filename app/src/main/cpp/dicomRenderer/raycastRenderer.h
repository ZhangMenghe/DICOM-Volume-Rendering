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
    GLuint vao_cube_= 0, vao_screen_ = 0;
    bool DRAW_BAKED;

    // for screen baking
    Texture* ray_baked_screen = nullptr;
    bool baked_dirty_ = true;
    int BAKED_RAY_SCREEN_ID;

    void on_create();
    void precompute();
    void DrawBaked();
public:
    raycastRenderer(){DRAW_BAKED = false;on_create();}
    raycastRenderer(bool screen_baked){DRAW_BAKED = screen_baked;on_create();}

    void onCuttingChange(float percent);
    void Draw();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
#include "cuttingController.h"
class raycastRenderer{
private:
    cuttingController* cutter_;

    Shader* shader_= nullptr, *geoshader_ = nullptr;
    GLuint VAO_=0;
    bool baked_dirty_ = true;
    Texture* bake_tex_ = nullptr;

public:
    raycastRenderer();
    void onCuttingChange(float percent);
    void Draw();
    void precompute();
};
#endif
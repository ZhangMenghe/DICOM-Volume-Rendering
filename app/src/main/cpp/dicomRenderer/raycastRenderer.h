#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include "cuttingController.h"
class raycastRenderer{
private:
    cuttingController* cutter_;

    Shader* shader_= nullptr;
    GLuint VAO_=0;

public:
    raycastRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
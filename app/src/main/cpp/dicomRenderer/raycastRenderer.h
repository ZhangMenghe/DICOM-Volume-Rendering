#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <cstring>
#include <GLES3/gl32.h>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>

class raycastRenderer{
private:
    Shader* shader_;
    GLuint VAO_;
public:
    raycastRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
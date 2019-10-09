#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#define GLM_ENABLE_EXPERIMENTAL

#include <cstring>
#include <GLES3/gl32.h>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>

class raycastRenderer{
private:
    Shader* shader_= nullptr, *cplane_shader_ = nullptr;
    GLuint VAO_=0, _cplaneVAO=0;
    glm::vec3 cplane_start_ = glm::vec3(-100.0f), cplane_normal = glm::vec3(.0f); //object space!!!
    float cplane_percent_ = .0f;

    void draw_cutting_plane();
public:
    raycastRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
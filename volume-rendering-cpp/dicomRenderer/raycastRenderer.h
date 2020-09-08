#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
class raycastRenderer{
private:
    Shader* shader_= nullptr, *cshader_ = nullptr;
    GLuint vao_cube_= 0;
    bool baked_dirty_ = true;

    void draw_baked(glm::mat4 model_mat);
    void draw_scene(glm::mat4 model_mat);
public:
    raycastRenderer();
    void Draw(bool pre_draw, glm::mat4 model_mat);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
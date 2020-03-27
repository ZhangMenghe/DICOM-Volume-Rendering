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
    GLuint frame_buff_ = 0;
    bool DRAW_BAKED;
    bool baked_dirty_ = true;
    glm::mat4 dim_scale_mat = glm::mat4(1.0);

    void draw_baked();
    void draw_scene();
public:
    raycastRenderer(bool screen_baked = true);

    void onCuttingChange(float percent);
    void Draw();
    void updatePrecomputation(GLuint sp);
    void dirtyPrecompute(){baked_dirty_ = true;}
    void setDimension(int dims);
};
#endif
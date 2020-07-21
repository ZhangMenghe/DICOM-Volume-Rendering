#ifndef CENTER_LINE_RENDERER_H
#define CENTER_LINE_RENDERER_H

#include <cstring>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class centerLineRenderer{
private:
    const int MAX_POINT_NUM=100;
    GLuint vao_, vbo_;
    bool DRAW_BAKED;
    int draw_point_num;

    Shader shader_;
    GLuint frame_buff_ = 0;


    //for screen baking
    bool baked_dirty_ = true;
    void draw_scene(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);

public:
    centerLineRenderer(bool screen_baked = true);
    void updateVertices(int point_num, const float * data);
    void onDraw(glm::mat4 model_mat);
    void dirtyPrecompute(){baked_dirty_ = true;}
};
#endif
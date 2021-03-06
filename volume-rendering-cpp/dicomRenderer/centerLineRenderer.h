#ifndef CENTER_LINE_RENDERER_H
#define CENTER_LINE_RENDERER_H

#include <cstring>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class centerLineRenderer{
private:
    const int MAX_POINT_NUM=4000;
    GLuint vao_, vbo_;
    int draw_point_num;

    Shader shader_;
    GLuint frame_buff_ = 0;
    int uid;

    //for screen baking
    bool baked_dirty_;
    void draw_scene(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);

public:
    centerLineRenderer(int id);
    void updateVertices(int point_num, const float * data);
    void onDraw(bool pre_draw, glm::mat4 model_mat);
    void dirtyPrecompute(){baked_dirty_ = true;}
    bool isPrecomputeDirty(){return baked_dirty_;}
};
#endif
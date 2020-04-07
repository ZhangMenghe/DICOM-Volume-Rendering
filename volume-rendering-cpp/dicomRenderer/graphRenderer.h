#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H

#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>

class GraphRenderer{
public:
    GraphRenderer();
    void setRelativeRenderRect(float w, float h, float left, float bottom);
    void updateVertices();
    void Draw();
private:
    GLuint vao_, vbo_;
    Shader shader_;
    void update_func_points(glm::vec2 p1, glm::vec2 p2);
};
#endif
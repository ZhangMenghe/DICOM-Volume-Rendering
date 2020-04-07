#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H

#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>
#include <dicomRenderer/basequadRenderer.h>

class GraphRenderer:public baseQuad{
public:
    GraphRenderer(std::string vertex_shader, std::string frag_shader);

    //override
//    void updateVertices(float* vertices);
    void Draw();
private:
//    void update_func_points(glm::vec2 p1, glm::vec2 p2);
};
#endif
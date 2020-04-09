#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H
#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>

#include <dicomRenderer/basequadRenderer.h>
class GraphRenderer:public baseQuad{
public:
    GraphRenderer(std::string vertex_shader, std::string frag_shader);
    static void getGraphPoints(float values[], float* &points);
    void setUniform(const char* key, const int count, float* data);
    void Draw();
};
#endif
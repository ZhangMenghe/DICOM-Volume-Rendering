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

private:
    const GLuint single_indices_[12]={0,2,1,0,5,2,0,4,5,0,3,4};
    GLuint indices[MAX_INSTANCES * 12] = {0};
    int num_of_instances;
};
#endif
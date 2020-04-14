#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H
#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>

#include <dicomRenderer/basequadRenderer.h>
class GraphRenderer:public baseQuad{
public:
    GraphRenderer(std::string vertex_shader, std::string frag_shader);
    static void getGraphPoints(float values[], float* &points);
    void Draw();
    void setData(std::vector<float*> data);
    void setData(float* data, int wid);
    void removeInstance(int wid);
    void addInstance();
    void Clear();
private:
    const GLuint single_indices_[12]={0,2,1,0,5,2,0,4,5,0,3,4};
    std::vector<GLuint> vaos_, vbos_;
};
#endif
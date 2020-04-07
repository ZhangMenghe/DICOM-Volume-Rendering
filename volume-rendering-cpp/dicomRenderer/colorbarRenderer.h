#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H

#include <dicomRenderer/basequadRenderer.h>
#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>

class ColorbarRenderer:public baseQuad{
public:
    ColorbarRenderer(std::string vertex_shader, std::string frag_shader);
//    void setRelativeRenderRect(float w, float h, float left, float bottom);
//    void updateVertices(float* vertices){}
    void Draw();
};
#endif
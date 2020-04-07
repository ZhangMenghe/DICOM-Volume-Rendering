#ifndef BASE_QUAD_RENDERER_H
#define BASE_QUAD_RENDERER_H

#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>

class baseQuad{
public:
    baseQuad(std::string vertex_shader, std::string frag_shader){
        if(!shader_.AddShader(GL_VERTEX_SHADER, vertex_shader)
           ||!shader_.AddShader(GL_FRAGMENT_SHADER,  frag_shader)
           ||!shader_.CompileAndLink())
            LOGE("FuncsVisual===Failed to create opacity shader program===");
    }
    void setRelativeRenderRect(float w, float h, float left, float bottom){
        GLuint sp = shader_.Use();
        Shader::Uniform(sp, "uScale", glm::vec2(w*2.0f, h*2.0f));
        Shader::Uniform(sp, "uOffset", glm::vec2(left, -1.0f + bottom * 2.0));
        shader_.UnUse();
    }
    void updateVertices(float* vertices, int data_size){
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    Shader* getShader(){return &shader_;}
    virtual void Draw()=0;

protected:
    GLuint vao_, vbo_;
    Shader shader_;
};
#endif
#ifndef BASE_QUAD_RENDERER_H
#define BASE_QUAD_RENDERER_H

#include <GLES3/gl32.h>
#include <GLPipeline/Shader.h>
#include <algorithm>

class baseQuad{
public:
    baseQuad(std::string vertex_shader, std::string frag_shader){
        if(!shader_.AddShader(GL_VERTEX_SHADER, vertex_shader)
           ||!shader_.AddShader(GL_FRAGMENT_SHADER, frag_shader)
           ||!shader_.CompileAndLink())
            LOGE("FuncsVisual===Failed to create opacity shader program===");
    }
    virtual void setRelativeRenderRect(float w, float h, float left, float bottom){
        r_scale_ = glm::vec2(w*2.0f, h*2.0f); r_offset_=glm::vec2(left, -1.0f + bottom * 2.0);
    }
    virtual void setUniform(const char* key, const int count, float* data){
        GLuint sp = shader_.Use();
        Shader::Uniform(sp, key, count, data);
        shader_.UnUse();
    }
    virtual void setUniform(const char* key, int id){
        GLuint sp = shader_.Use();
        Shader::Uniform(sp, key, id);
        shader_.UnUse();
    }
    virtual void setUniform(const char* key, float v){
        GLuint sp = shader_.Use();
        Shader::Uniform(sp, key, v);
        shader_.UnUse();
    }
    virtual void setData(std::vector<float*> data){}
    virtual void setData(float* data, int wid){}
    virtual void Clear(){}
    virtual void Draw()=0;

protected:
    Shader shader_;
    glm::vec2 r_scale_, r_offset_;
    const static int MAX_INSTANCES = 5;
};
#endif
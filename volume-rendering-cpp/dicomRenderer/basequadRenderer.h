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
    void setUniform(const char* key, int id){
        GLuint sp = shader_.Use();
        Shader::Uniform(sp, key, id);
        shader_.UnUse();
    }
    void setUniforms(float values[]){
        glm::vec2 lb, lm, lt, rb, rm, rt;
        float half_top = values[2] / 2.0f;
        float half_bottom = std::max(values[1] / 2.0f, half_top);

        float raw_lb_x = values[3] - half_bottom;
        float raw_rb_x = values[3] + half_bottom;
        float lb_x = std::max(.0f, raw_lb_x);
        float rb_x = std::min(1.0f, raw_rb_x);
        lb = glm::vec2(lb_x, .0f);
        rb = glm::vec2(rb_x, .0f);

        lt = glm::vec2(std::max(.0f, values[3] - half_top), values[0]);
        rt = glm::vec2(std::min(1.0f, values[3] + half_top), values[0]);

        float mid_y = values[4] * values[0];
        if(lb_x == .0)lm = glm::vec2(lb_x,lt.y/(lt.x - raw_lb_x) * (-raw_lb_x));
        else lm = glm::vec2(lb_x, mid_y);
        if(rb_x == 1.0)rm = glm::vec2(rb_x, rt.y / (rt.x - raw_rb_x) *(1-raw_rb_x));
        else rm = glm::vec2(rb_x, mid_y);
        
        float vertices[12] = {
            lb.x, lb.y, lm.x, lm.y, lt.x, lt.y,
            rb.x, rb.y, rm.x, rm.y, rt.x, rt.y
        };
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 12* sizeof(float), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    virtual void Draw()=0;

protected:
    GLuint vao_, vbo_;
    Shader shader_;
//    const int OPACITY_VALUE_NUM = 4;
//    const char* opacity_names[OPACITY_VALUE_NUM] = {
//        "uOpacitys.overall",
//        "uOpacitys.bottom_width",
//        "uOpacitys.top_width",
//        "uOpacitys.center"
//          "lowest"
//    };
};
#endif
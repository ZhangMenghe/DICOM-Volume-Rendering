#ifndef COLORBAR_RENDERER_H
#define COLORBAR_RENDERER_H

#include <dicomRenderer/basequadRenderer.h>
#include <GLPipeline/Texture.h>
class ColorbarRenderer:public baseQuad{
public:
    ColorbarRenderer(std::string vertex_shader, std::string frag_shader);
    void Draw();
    void setUniform(const char* key, const int count, float* data, int unit_size = 2){
        GLuint sp = m_compute_shader->Use();
        Shader::Uniform(sp, key, count, unit_size, data);
        m_compute_shader->UnUse();
    }
    void setUniform(const char* key, const int count, const int* data){
        GLuint sp = m_compute_shader->Use();
        Shader::Uniform(sp, key, count, data);
        m_compute_shader->UnUse();
    }
    void setUniform(const char* key, int id){
        GLuint sp = m_compute_shader->Use();
        Shader::Uniform(sp, key, id);
        m_compute_shader->UnUse();
    }
    void setUniform(const char* key, float v){
        GLuint sp = m_compute_shader->Use();
        Shader::Uniform(sp, key, v);
        m_compute_shader->UnUse();
    }
private:
    GLuint vao_;
    Shader* m_compute_shader;
    Texture* m_cv_tex;
    GLuint tex_width=256, tex_height=120;
};
#endif
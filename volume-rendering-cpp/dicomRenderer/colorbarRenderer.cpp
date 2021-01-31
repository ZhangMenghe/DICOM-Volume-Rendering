#include <GLPipeline/Mesh.h>
#include <GLPipeline/Primitive.h>
#include <dicomRenderer/colorbarRenderer.h>
#include <vrController.h>

ColorbarRenderer::ColorbarRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
    Mesh::InitQuadWithTex(vao_, quad_vertices_tex, 4, quad_indices, 6);
    m_compute_shader = new Shader;
    if(!m_compute_shader->AddShader(GL_COMPUTE_SHADER, Manager::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG])
       ||!m_compute_shader->CompileAndLink())
        LOGE("COLOR VIZ=====Failed to create raycast geometry shader");
    Manager::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG]="";

    auto vsize = tex_width* tex_height * 4;
    GLbyte * vdata = new GLbyte[vsize];
    memset(vdata, 0xff, vsize * sizeof(GLbyte));
    m_cv_tex = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, tex_width, tex_height, vdata);
}

void ColorbarRenderer::Draw(){
    m_compute_shader->DisableAllKeyword();
    m_compute_shader->EnableKeyword(dvr::COLOR_SCHEMES[Manager::color_scheme_id]);

    GLuint sp = m_compute_shader->Use();
    glBindImageTexture(0, m_cv_tex->GLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    if(Manager::color_scheme_id > 2)
        Shader::Uniform(sp, "u_hex_color_scheme", 256, color_schemes_hex[Manager::color_scheme_id- 3]);
    Shader::Uniform(sp, "u_tex_size", glm::vec2(tex_width,tex_height));

    glDispatchCompute((GLuint)(tex_width + 7) / 8, 1,1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
    m_compute_shader->UnUse();

    //draw
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint dsp = shader_.Use();
    Shader::Uniform(dsp, "uScale", r_scale_);
    Shader::Uniform(dsp, "uOffset", r_offset_);

    glActiveTexture(GL_TEXTURE0 + dvr::COLOR_VIZ_QUAD_TEX_ID);
    glBindTexture(GL_TEXTURE_2D, m_cv_tex->GLTexture());
    Shader::Uniform(dsp, "uSampler", dvr::COLOR_VIZ_QUAD_TEX_ID);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
    glDisable(GL_BLEND);
}
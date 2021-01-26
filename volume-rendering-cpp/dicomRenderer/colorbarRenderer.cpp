#include <GLPipeline/Mesh.h>
#include <GLPipeline/Primitive.h>
#include <dicomRenderer/colorbarRenderer.h>
#include <vrController.h>

ColorbarRenderer::ColorbarRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
    Mesh::InitQuadWithTex(vao_, quad_vertices_tex, 4, quad_indices, 6);
}

void ColorbarRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", r_scale_);
    Shader::Uniform(sp, "uOffset", r_offset_);
    if(Manager::color_scheme_id > 2)
        Shader::Uniform(sp, "u_hex_color_scheme", 256, color_schemes_hex[Manager::color_scheme_id - 3]);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
    glDisable(GL_BLEND);
}
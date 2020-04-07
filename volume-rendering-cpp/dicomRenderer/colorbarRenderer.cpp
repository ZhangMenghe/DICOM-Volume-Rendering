#include <GLPipeline/Mesh.h>
#include <GLPipeline/Primitive.h>
#include <dicomRenderer/colorbarRenderer.h>

ColorbarRenderer::ColorbarRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
    Mesh::InitQuadWithTex(vao_, quad_vertices_tex, 4, quad_indices, 6);
}

void ColorbarRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader_.Use();
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
    glDisable(GL_BLEND);
}
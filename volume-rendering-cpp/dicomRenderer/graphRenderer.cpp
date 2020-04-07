#include <GLPipeline/Primitive.h>
#include <GLPipeline/Mesh.h>
#include "graphRenderer.h"
GraphRenderer::GraphRenderer(std::string vertex_shader, std::string frag_shader)
:baseQuad(vertex_shader, frag_shader){

    Mesh::InitQuad(vao_, vbo_, quad_indices, 6);
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", glm::vec2(1.8f, .15f));
    Shader::Uniform(sp, "uColor", glm::vec4(0.678f, 0.839f, 0.969f, 0.5f));
    shader_.UnUse();
}

void GraphRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader_.Use();
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
    glDisable(GL_BLEND);
}
//void GraphRenderer::updateVertices(float* vertices){
//    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*4 *sizeof(GL_FLOAT), vertices);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
////    if(vrController::param_bool[dvr::CHECK_RAYCAST]){
////        update_func_points(glm::vec2(vrController::param_ray[dvr::TR_CUTOFF]-0.5f,
////                          vrController::param_ray[dvr::TR_LOWEST] * vrController::param_ray[dvr::TR_OVERALL]),
////                glm::vec2(0.5f, vrController::param_ray[dvr::TR_OVERALL]));
////    }else{update_func_points(glm::vec2(vrController::param_tex[dvr::TT_CUTOFF]-0.5f,
////                          vrController::param_tex[dvr::TT_LOWEST] * vrController::param_tex[dvr::TT_OVERALL]),
////                glm::vec2(0.5f, vrController::param_tex[dvr::TT_OVERALL]));
////    }
//}
//void GraphRenderer::update_func_points(glm::vec2 p1, glm::vec2 p2){
//    float vertices[] = {
//            p2.x, p2.y, .0f,//top-right
//            p1.x, p1.y,.0f,//top-left
//            p1.x, .0f,.0f,//bottom-left
//            0.5f, .0f, .0f,//bottom-right
//    };
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*4 *sizeof(GL_FLOAT), vertices);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

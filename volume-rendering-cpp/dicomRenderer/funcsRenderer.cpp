#include <cstring>
#include <vrController.h>
#include "funcsRenderer.h"
#include <GLPipeline/Primitive.h>
FuncRenderer::FuncRenderer(){
    vertices_func_ = new GLfloat[MAX_VERTICS * 3];

    if(!shader_func.AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_OPA_VIZ_VERT])
       ||!shader_func.AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_OPA_VIZ_FRAG])
       ||!shader_func.CompileAndLink())
        LOGE("FuncsVisual===Failed to create opacity shader program===");

    if(!shader_quad.AddShader(GL_VERTEX_SHADER,vrController::shader_contents[dvr::SHADER_OPA_VIZ_VERT])
       ||!shader_quad.AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_OPA_VIZ_FRAG])
       ||!shader_quad.CompileAndLink())
        LOGE("FuncsVisual===Failed to create shader_quad shader program===");

    if(!shader_colorbar.AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_COLOR_VIZ_VERT])
       ||!shader_colorbar.AddShader(GL_FRAGMENT_SHADER, vrController::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG])
       ||!shader_colorbar.CompileAndLink())
        LOGE("FuncsVisual===Failed to create shader_colorbar shader program===");
    vrController::shader_contents[dvr::SHADER_OPA_VIZ_VERT] = "";vrController::shader_contents[dvr::SHADER_OPA_VIZ_FRAG]="";
    vrController::shader_contents[dvr::SHADER_COLOR_VIZ_VERT] = "";vrController::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG]="";

    GLuint sp = shader_func.Use();
    Shader::Uniform(sp, "uScale", glm::vec2(1.8f, .15f));
    Shader::Uniform(sp, "uColor", glm::vec4(0.678f, 0.839f, 0.969f, 0.5f));
    shader_func.UnUse();

    sp = shader_quad.Use();
    Shader::Uniform(sp,"uScale", glm::vec2(2.0f, 0.20f));
    Shader::Uniform(sp,"uColor", glm::vec4(0.086f, 0.098f, 0.231f, 1.0f));
    shader_quad.UnUse();

    sp = shader_colorbar.Use();
    Shader::Uniform(sp,"uScale", glm::vec2(2.0f, 0.1f));
    Shader::Uniform(sp,"uOffset", glm::vec2(.0f, -1.0f));
    shader_colorbar.UnUse();
}

void FuncRenderer::CreateFunction(FUNC_TYPE type){
    switch(type){
        case COLOR_BAR:
            Mesh::InitQuadWithTex(VAO_COLOR_BAR, quad_vertices_tex, 4, quad_indices, 6);
            break;
        case OPACITY_FUN:
            Mesh::InitQuad(VAO_FUNC, VBO_FUNC, quad_indices,6);
            Mesh::InitQuad(VAO_QUAD, quad_vertices, 4, quad_indices, 6);
            UpdateFuncPoints(OPACITY_FUN);
            break;
        default:
            break;
    }
}
void FuncRenderer::UpdateFuncPoints(FUNC_TYPE type){
    if(type == OPACITY_FUN){
        if(vrController::param_bool[dvr::CHECK_RAYCAST]){
            UpdateFuncPoints(
                    OPACITY_FUN,
                    glm::vec2(vrController::param_ray[dvr::TR_CUTOFF]-0.5f,
                              vrController::param_ray[dvr::TR_LOWEST] * vrController::param_ray[dvr::TR_OVERALL]),
                    glm::vec2(0.5f, vrController::param_ray[dvr::TR_OVERALL]));
        }else{
            UpdateFuncPoints(
                    OPACITY_FUN,
                    glm::vec2(vrController::param_tex[dvr::TT_CUTOFF]-0.5f,
                              vrController::param_tex[dvr::TT_LOWEST] * vrController::param_tex[dvr::TT_OVERALL]),
                    glm::vec2(0.5f, vrController::param_tex[dvr::TT_OVERALL]));
        }
    }

}
void FuncRenderer::UpdateFuncPoints(FUNC_TYPE type, glm::vec2 p1, glm::vec2 p2, bool is_quad){
    float vertices[] = {
            p2.x, p2.y, .0f,//top-right
            p1.x, p1.y,.0f,//top-left
            p1.x, .0f,.0f,//bottom-left
            0.5f, .0f, .0f,//bottom-right
    };
    if(is_quad)
        memcpy(vertices_func_, vertices, 3*4*sizeof(GL_FLOAT));
    else
        memcpy(vertices_func_, vertices, 3*2*sizeof(GL_FLOAT));
}
void FuncRenderer::UpdateFuncPoints(FUNC_TYPE type, float* points, bool is_quad){
    if(is_quad)
        memcpy(vertices_func_, points, 3*4*sizeof(GL_FLOAT));
    else
        memcpy(vertices_func_, points, 3*2*sizeof(GL_FLOAT));
}
void FuncRenderer::draw_color_bar(){
    shader_colorbar.Use();
        glBindVertexArray(VAO_COLOR_BAR);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
}
void FuncRenderer::draw_opacity_func(){
    float trans_color_offset = .0f;
    if(vrController::color_scheme_id == 1)
        trans_color_offset = 0.08f;
//    if(!vrController::param_bool[dvr::CHECK_RAYCAST])
        UpdateFuncPoints(OPACITY_FUN);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_FUNC);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*4  *sizeof(GL_FLOAT), vertices_func_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint sp = shader_quad.Use();
    Shader::Uniform(sp, "uOffset", .0f, -1.0+trans_color_offset);

        glBindVertexArray(VAO_QUAD);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    shader_quad.UnUse();

    sp = shader_func.Use();
    Shader::Uniform(sp, "uOffset", .0f, -1.0f+trans_color_offset);
        glBindVertexArray(VAO_FUNC);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    shader_func.UnUse();

    glDisable(GL_BLEND);
}
void FuncRenderer::Draw(){
    if(vrController::color_scheme_id == 1)draw_color_bar();
    draw_opacity_func();

}

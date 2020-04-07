//#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include <dicomRenderer/colorbarRenderer.h>

ColorbarRenderer::ColorbarRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
//    Mesh::InitQuadWithTex(vao_, quad_vertices_tex, 4, quad_indices, 6);
//    if(!shader_.AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_COLOR_VIZ_VERT])
//       ||!shader_.AddShader(GL_FRAGMENT_SHADER, vrController::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG])
//       ||!shader_.CompileAndLink())
//        LOGE("FuncsVisual===Failed to create shader_colorbar shader program===");
//    vrController::shader_contents[dvr::SHADER_OPA_VIZ_VERT] = "";vrController::shader_contents[dvr::SHADER_OPA_VIZ_FRAG]="";
//    vrController::shader_contents[dvr::SHADER_COLOR_VIZ_VERT] = "";vrController::shader_contents[dvr::SHADER_COLOR_VIZ_FRAG]="";

}

void ColorbarRenderer::Draw(){

}
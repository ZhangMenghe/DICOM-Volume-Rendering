#include <GLPipeline/Mesh.h>
#include "cutplaneRenderer.h"
#include <GLPipeline/Primitive.h>
#include <Manager.h>
#include <dicomRenderer/Constants.h>
#include <GLPipeline/Texture.h>
#include <dicomRenderer/screenQuad.h>
#include <vrController.h>

cutplaneRenderer::cutplaneRenderer(bool screen_baked)
:DRAW_TO_TEXTURE(screen_baked){
    Mesh::InitQuadWithTex(vao_, cuboid_with_texture, 8, cuboid_indices, 36);
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_CUT_PLANE_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_CUT_PLANE_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("AR Cutting plane===Failed to create raycast shader program===");
    Manager::shader_contents[dvr::SHADER_CUT_PLANE_VERT] = "";Manager::shader_contents[dvr::SHADER_CUT_PLANE_FRAG]="";
    scale_mat_ = glm::scale(glm::mat4(1.0), glm::vec3(4.0, 4.0, 1.0f));
}

void cutplaneRenderer::Draw(glm::mat4 viewproj_mat, glm::vec3 position, glm::vec3 normal_vec){
    if(!Manager::show_ar_ray || !Manager::isRayCut()) return;

    glm::vec3 dir = vrController::instance()->getModelPos() - Manager::camera->getCameraPosition();

    glm::mat4 model_mat = glm::translate(glm::mat4(1.0), position)
            *Manager::camera->getRotationMatrixOfCameraDirection()
//            *glm::rotate(glm::mat4(1.0f), 0.1f, glm::vec3(0,1,0))
            *scale_mat_;
    if(DRAW_TO_TEXTURE) draw_to_texture(viewproj_mat*model_mat, model_mat, normal_vec);
    else draw_scene(viewproj_mat*model_mat, model_mat, normal_vec);
}

void cutplaneRenderer::draw_to_texture(glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec){
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(mvp, model_mat, normal_vec);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void cutplaneRenderer::draw_scene(glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    GLuint sp = shader_->Use();
    Shader::Uniform(sp, "uMVP", mvp);
    Shader::Uniform(sp, "uModelMat", model_mat);
    Shader::Uniform(sp, "uNormal", normal_vec);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_->UnUse();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

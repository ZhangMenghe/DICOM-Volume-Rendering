#include <vrController.h>
#include "raycastRenderer.h"
#include "screenQuad.h"
#include <GLPipeline/Primitive.h>
#include <glm/gtx/string_cast.hpp>
raycastRenderer::raycastRenderer(bool screen_baked):
DRAW_BAKED(screen_baked){
    //geometry
    Mesh::InitQuadWithTex(vao_cube_, cuboid_with_texture, 8, cuboid_indices, 36);

    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_VERT])
            ||!shader_->AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_FRAG])
            ||!shader_->CompileAndLink())
        LOGE("Raycast===Failed to create raycast shader program===");
    Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_VERT] = "";Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_FRAG]="";
}
void raycastRenderer::Draw(glm::mat4 model_mat) {
    if (DRAW_BAKED)//&&!Manager::param_bool[dvr::CUT_CUTTING_PLANE])
        draw_baked(model_mat);
    else draw_scene(model_mat);
}

void raycastRenderer::draw_scene(glm::mat4 model_mat){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //Update cutting plane and draw
    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
    Shader::Uniform(sp, "uSampler", dvr::BAKED_TEX_ID);
    Shader::Uniform(sp, "uVPMat", Manager::camera->getProjMat()*Manager::camera->getViewMat());

    glm::mat4 model_inv = glm::inverse(model_mat);

    Shader::Uniform(sp, "uModelMat", model_mat);
    Shader::Uniform(sp, "uCamposObjSpace",
            glm::vec3(model_inv*glm::vec4(Manager::camera->getCameraPosition(), 1.0)));
//    Shader::Uniform(sp,"sample_step_inverse", 1.0f / Manager::param_ray[dvr::TR_DENSITY]);
    Shader::Uniform(sp,"usample_step_inverse", 1.0f/600.0f);
    
    if(Manager::IsCuttingEnabled())shader_->EnableKeyword("CUTTING_PLANE");
    else shader_->DisableKeyword("CUTTING_PLANE");

    vrController::instance()->setCuttingParams(sp, false);

    //for backface rendering! don't erase
    glm::mat4 rotmat = vrController::instance()->getRotationMatrix();
//    glm::vec3 dir = glm::vec3(rotmat[0][2], rotmat[1][2],rotmat[2][2]);
//    if(glm::dot(Manager::camera->getViewDirection(), dir) < 0) glFrontFace(GL_CCW);
    if(rotmat[2][2] > 0) glFrontFace(GL_CCW);
    else glFrontFace(GL_CW);

    glBindVertexArray(vao_cube_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_->UnUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void raycastRenderer::draw_baked(glm::mat4 model_mat){
    if(!baked_dirty_) {screenQuad::instance()->Draw(); return;}
    if(!cshader_){
        cshader_ = new Shader;
        if(!cshader_->AddShader(GL_COMPUTE_SHADER, Manager::shader_contents[dvr::SHADER_RAYCASTCOMPUTE_GLSL])
           ||!cshader_->CompileAndLink())
            LOGE("Raycast=====Failed to create raycast geometry shader");
        Manager::shader_contents[dvr::SHADER_RAYCASTCOMPUTE_GLSL]="";
    }

    if(Manager::IsCuttingEnabled())cshader_->EnableKeyword("CUTTING_PLANE");
    else cshader_->DisableKeyword("CUTTING_PLANE");

    GLuint sp = cshader_->Use();
    Texture* ray_baked_screen  = screenQuad::instance()->getTex();
    glBindImageTexture(0, vrController::instance()->getBakedTex(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);//GL_RGBA8);
    glBindImageTexture(1, ray_baked_screen->GLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_con_size", screenQuad::instance()->getTexSize());
    Shader::Uniform(sp, "u_fov", Manager::camera->getFOV());

    glm::mat4 model_inv = glm::inverse(model_mat);
    Shader::Uniform(sp, "u_WorldToModel", model_inv);
    Shader::Uniform(sp, "u_CamToWorld", glm::translate(glm::mat4(1.0), Manager::camera->getCameraPosition()));
    Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv*glm::vec4(Manager::camera->getCameraPosition(), 1.0)));
    Shader::Uniform(sp, "usample_step_inverse", 1.0f / 600.0f);
    //todo
    vrController::instance()->setCuttingParams(sp, true);

    glDispatchCompute((GLuint)(ray_baked_screen->Width() + 7) / 8, (GLuint)(ray_baked_screen->Height() + 7) / 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    cshader_->UnUse();
    baked_dirty_ = false;

    //todo: draw screen quad
    screenQuad::instance()->Draw();
}

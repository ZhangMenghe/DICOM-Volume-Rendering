#include <vrController.h>
#include "raycastRenderer.h"
#include "screenQuad.h"
#include <GLPipeline/Primitive.h>
raycastRenderer::raycastRenderer(bool screen_baked):
DRAW_BAKED(screen_baked){
    //geometry
    Mesh::InitQuadWithTex(vao_cube_, cuboid_with_texture, 8, cuboid_indices, 36);

    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER,vrController::shader_contents[dvr::SHADER_RAYCASTVOLUME_VERT])
            ||!shader_->AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_RAYCASTVOLUME_FRAG])
            ||!shader_->CompileAndLink())
        LOGE("Raycast===Failed to create raycast shader program===");
    cutter_ = new cuttingController;
}

void raycastRenderer::Draw() {
    if (DRAW_BAKED) draw_baked();
    else if(vrController::param_bool[dvr::CHECK_ARENABLED]) draw_to_texture();
    else draw_scene();
}
void raycastRenderer::draw_scene() {
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_DEPTH_TEST);

   //Update cutting plane and draw
   cutter_->UpdateAndDraw();
   Texture* tex = vrController::instance()->getTex(dvr::VOLUME_TEX_ID);

   GLuint sp = shader_->Use();
//    if(vrController::ray_baked){
//        glActiveTexture(GL_TEXTURE0+dvr::BAKED_RAY_ID);
//        glBindTexture(GL_TEXTURE_3D, vrController::ray_baked->GLTexture());
//        Shader::Uniform(sp, "uSampler_tex", dvr::BAKED_RAY_ID);
//    }else{
       glActiveTexture(GL_TEXTURE0+dvr::VOLUME_TEX_ID);
       glBindTexture(GL_TEXTURE_3D, tex->GLTexture());
       Shader::Uniform(sp, "uSampler_tex", dvr::VOLUME_TEX_ID);
//    }
   Shader::Uniform(sp, "uProjMat", vrController::camera->getProjMat());
   Shader::Uniform(sp,"uViewMat", vrController::camera->getViewMat());
   Shader::Uniform(sp,"uModelMat", vrController::ModelMat_);

       glm::mat4 model_inv = glm::inverse(vrController::ModelMat_ * dim_scale_mat);
   Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv
       *glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
   Shader::Uniform(sp,"uVolumeSize",
               glm::vec3(tex->Width(),
               tex->Height(),
               tex->Depth()));

   Shader::Uniform(sp,"ub_cuttingplane", vrController::param_bool[dvr::CHECK_CUTTING]);

   Shader::Uniform(sp,"sample_step_inverse", 1.0f / vrController::param_ray[dvr::TR_DENSITY]);
   cutter_->setCuttingParams(sp);

   if(vrController::camera->getViewDirection().z <0)
       glFrontFace(GL_CW);
   else
       glFrontFace(GL_CCW);

       glBindVertexArray(vao_cube_);
       glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
       glBindVertexArray(0);
   shader_->UnUse();


   glDisable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
}

void raycastRenderer::onCuttingChange(float percent){
    cutter_->setCutPlane(percent);
//    dirtyPrecompute();
}

void raycastRenderer::updatePrecomputation(GLuint sp){
    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_ray[dvr::TR_OVERALL]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_ray[dvr::TR_LOWEST]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_ray[dvr::TR_CUTOFF]);
}

void raycastRenderer::draw_baked(){
    if(!baked_dirty_) return;
    if(!cshader_){
        cshader_ = new Shader;
        if(!cshader_->AddShader(GL_COMPUTE_SHADER, vrController::shader_contents[dvr::SHADER_RAYCASTCOMPUTE_GLSL])
           ||!cshader_->CompileAndLink())
            LOGE("Raycast=====Failed to create raycast geometry shader");
    }

    if(vrController::param_bool[dvr::CHECK_CUTTING])cshader_->EnableKeyword("CUTTING_PLANE");
    else cshader_->DisableKeyword("CUTTING_PLANE");
    cshader_->EnableKeyword("ENABLE_AR");

    GLuint sp = cshader_->Use();
    Texture* ray_baked_screen  = screenQuad::instance()->getTex();
    glBindImageTexture(0, vrController::instance()->getTex(dvr::BAKED_RAY_ID)->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16UI);//GL_RGBA8);
    glBindImageTexture(1, screenQuad::instance()->getTex()->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(2, ray_baked_screen->GLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);


    Shader::Uniform(sp, "u_con_size", screenQuad::instance()->getTexSize());
    Shader::Uniform(sp, "u_fov", vrController::camera->getFOV());

    glm::mat4 model_inv = glm::inverse(vrController::ModelMat_ * dim_scale_mat);

    Shader::Uniform(sp, "u_WorldToModel", model_inv);
//    if(vrController::param_bool[dvr::CHECK_ARENABLED])
        Shader::Uniform(sp, "u_CamToWorld", vrController::camera->getCameraPose());
//    else
//        Shader::Uniform(sp, "u_CamToWorld", glm::translate(glm::mat4(1.0), cam_world_pos));

    Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
    Shader::Uniform(sp, "usample_step_inverse", 1.0f / vrController::param_ray[dvr::TR_DENSITY]);
    cutter_->Update();
    cutter_->setCuttingParams(sp, true);

    glDispatchCompute((GLuint)(ray_baked_screen->Width() + 7) / 8, (GLuint)(ray_baked_screen->Height() + 7) / 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16UI);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(2, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    cshader_->UnUse();
    baked_dirty_ = false;
}
void raycastRenderer::draw_to_texture(){
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}
void raycastRenderer::setDimension(int dims){
    dim_scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, dims / 200.0f));
}
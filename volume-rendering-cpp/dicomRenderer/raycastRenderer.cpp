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
    vrController::shader_contents[dvr::SHADER_RAYCASTVOLUME_VERT] = "";vrController::shader_contents[dvr::SHADER_RAYCASTVOLUME_FRAG]="";

    cutter_ = new cuttingController;
}
void raycastRenderer::Draw() {
    if (DRAW_BAKED) draw_baked();
    else draw_scene();
}

void raycastRenderer::draw_scene(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //Update cutting plane and draw
    cutter_->UpdateAndDraw();
    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
    Shader::Uniform(sp, "uSampler", dvr::BAKED_TEX_ID);

    Shader::Uniform(sp, "uProjMat", vrController::camera->getProjMat());
    Shader::Uniform(sp, "uViewMat", vrController::camera->getViewMat());
    Shader::Uniform(sp, "uModelMat", vrController::instance()->getModelMatrix());

    glm::mat4 modelmat = vrController::instance()->getModelMatrix();
    glm::mat4 model_inv = glm::inverse(modelmat * dim_scale_mat);
    Shader::Uniform(sp, "uCamposObjSpace",
            glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
//    Shader::Uniform(sp,"sample_step_inverse", 1.0f / vrController::param_ray[dvr::TR_DENSITY]);
    Shader::Uniform(sp,"sample_step_inverse", 1.0f/400);
    if(vrController::param_bool[dvr::CHECK_CUTTING])shader_->EnableKeyword("CUTTING_PLANE");
    else shader_->DisableKeyword("CUTTING_PLANE");

    cutter_->setCuttingParams(sp);

    //for backface rendering! don't erase
    glm::mat4 rotmat = vrController::instance()->getRotationMatrix();
//    glm::vec3 dir = glm::vec3(rotmat[0][2], rotmat[1][2],rotmat[2][2]);
//    if(glm::dot(vrController::camera->getViewDirection(), dir) < 0) glFrontFace(GL_CCW);
    if(rotmat[2][2] > 0) glFrontFace(GL_CCW);
    else glFrontFace(GL_CW);

    glBindVertexArray(vao_cube_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_->UnUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}
void raycastRenderer::setCuttingPlane(glm::vec3 pp, glm::vec3 pn){
    cutter_->setCutPlane(pp, pn);
    baked_dirty_ = true;
}
void raycastRenderer::setCuttingPlane(float percent){
    cutter_->setCutPlane(percent);
    baked_dirty_ = true;
}
float* raycastRenderer::getCuttingPlane(){
    return cutter_->getCutPlane();
}
void raycastRenderer::draw_baked(){
    if(!baked_dirty_) {screenQuad::instance()->Draw(); return;}
    if(!cshader_){
        cshader_ = new Shader;
        if(!cshader_->AddShader(GL_COMPUTE_SHADER, vrController::shader_contents[dvr::SHADER_RAYCASTCOMPUTE_GLSL])
           ||!cshader_->CompileAndLink())
            LOGE("Raycast=====Failed to create raycast geometry shader");
        vrController::shader_contents[dvr::SHADER_RAYCASTCOMPUTE_GLSL]="";

    }

    if(vrController::param_bool[dvr::CHECK_CUTTING])cshader_->EnableKeyword("CUTTING_PLANE");
    else cshader_->DisableKeyword("CUTTING_PLANE");

    GLuint sp = cshader_->Use();
    Texture* ray_baked_screen  = screenQuad::instance()->getTex();
    glBindImageTexture(0, vrController::instance()->getBakedTex(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);//GL_RGBA8);
    glBindImageTexture(1, ray_baked_screen->GLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_con_size", screenQuad::instance()->getTexSize());
    Shader::Uniform(sp, "u_fov", vrController::camera->getFOV());

    glm::mat4 model_inv = glm::inverse(vrController::instance()->getModelMatrix() * dim_scale_mat);
    Shader::Uniform(sp, "u_WorldToModel", model_inv);
    Shader::Uniform(sp, "u_CamToWorld", glm::translate(glm::mat4(1.0), vrController::camera->getCameraPosition()));
    Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
    Shader::Uniform(sp, "usample_step_inverse", 1.0f / 600.0f);
    cutter_->Update();
    cutter_->setCuttingParams(sp, true);

    glDispatchCompute((GLuint)(ray_baked_screen->Width() + 7) / 8, (GLuint)(ray_baked_screen->Height() + 7) / 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    cshader_->UnUse();
    baked_dirty_ = false;

    //todo: draw screen quad
    screenQuad::instance()->Draw();
}
void raycastRenderer::setDimension(int dims, float thickness){
    if(thickness > 0 ){
        dim_scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, thickness));
    }else{
        if(dims > 200) dim_scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 0.5f));
        else if(dims > 100) dim_scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, dims / 300.f));
        else dim_scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, dims / 200.f));
    }
}

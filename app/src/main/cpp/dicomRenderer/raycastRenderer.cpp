#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include "raycastRenderer.h"
#include <GLPipeline/Primitive.h>
#include <AndroidUtils/mathUtils.h>
void raycastRenderer::on_create() {
    //geometry
    Mesh::InitQuadWithTex(vao_cube_, cuboid_with_texture, 8, cuboid_indices, 36);
    Mesh::InitQuadWithTex(vao_screen_, quad_vertices_tex_standard, 4, quad_indices, 6);

    //program
    shader_ = new Shader();
    if(!shader_->AddShaderFile(GL_VERTEX_SHADER,"shaders/raycastVolume.vert")
            ||!shader_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/raycastVolume.frag")
            ||!shader_->CompileAndLink())
        LOGE("Raycast===Failed to create raycast shader program===");


    shader_baked_ = new Shader();
    if(!shader_baked_->AddShaderFile(GL_VERTEX_SHADER,"shaders/quad.vert")
       ||!shader_baked_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/quad.frag")
       ||!shader_baked_->CompileAndLink())
        LOGE("Raycast===Failed to create raycast shader program===");

    cutter_ = new cuttingController;
}
void raycastRenderer::DrawBaked(){
    precompute();
    GLuint sp = shader_baked_->Use();
    glActiveTexture(GL_TEXTURE0+BAKED_RAY_SCREEN_ID);
    glBindTexture(GL_TEXTURE_2D, ray_baked_screen->GLTexture());
    Shader::Uniform(sp, "uSampler", BAKED_RAY_SCREEN_ID);
    glBindVertexArray(vao_screen_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_baked_->UnUse();
}
void raycastRenderer::Draw(){
    if(DRAW_BAKED) {DrawBaked(); return;}

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    //Update cutting plane and draw
    cutter_->UpdateAndDraw();

    GLuint sp = shader_->Use();
    if(vrController::ray_baked){
        glActiveTexture(GL_TEXTURE0+vrController::BAKED_RAY_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::ray_baked->GLTexture());
        Shader::Uniform(sp, "uSampler_tex", vrController::BAKED_RAY_ID);
    }else{
        glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());
        Shader::Uniform(sp, "uSampler_tex", vrController::VOLUME_TEX_ID);
    }
    Shader::Uniform(sp, "uProjMat", vrController::camera->getProjMat());
    Shader::Uniform(sp,"uViewMat", vrController::camera->getViewMat());
    Shader::Uniform(sp,"uModelMat", vrController::ModelMat_);

        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
    Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv
        *glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
    Shader::Uniform(sp,"uVolumeSize",
                glm::vec3(vrController::tex_volume->Width(),
                vrController::tex_volume->Height(),
                vrController::tex_volume->Depth()));

    Shader::Uniform(sp,"ub_accumulate", vrController::param_bool_map["accumulate"]);
    Shader::Uniform(sp,"ub_cuttingplane", vrController::param_bool_map["cutting"]);

    Shader::Uniform(sp,"sample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
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
//    glDisable(GL_CULL_FACE);
}

void raycastRenderer::onCuttingChange(float percent){
    cutter_->setCutPlane(percent);
//    dirtyPrecompute();
}
void raycastRenderer::updatePrecomputation(GLuint sp){
    Shader::Uniform(sp, "u_val_threshold", vrController::param_value_map["threshold"]);
    Shader::Uniform(sp, "u_brightness", vrController::param_value_map["brightness"]);
}
void raycastRenderer::precompute(){
    if(!baked_dirty_) return;
    GLenum err;
    if(!cshader_){
        cshader_ = new Shader;
        if(!cshader_->AddShaderFile(GL_COMPUTE_SHADER, "shaders/raycastCompute.glsl")
           ||!cshader_->CompileAndLink())
            LOGE("Raycast=====Failed to create raycast geometry shader");

        BAKED_RAY_SCREEN_ID = vrController::BAKED_RAY_ID + 1;

        float width = vrController::_screen_w, height = vrController::_screen_h;
        int vsize= width* height;
        GLbyte * vdata = new GLbyte[vsize * 4];
        memset(vdata, 0xff, vsize * 4 * sizeof(GLbyte));
        ray_baked_screen = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, vdata);
        delete[]vdata;
    }

    if(vrController::param_bool_map["cutting"])cshader_->EnableKeyword("CUTTING_PLANE");
    else cshader_->DisableKeyword("CUTTING_PLANE");

    GLuint sp = cshader_->Use();
    glBindImageTexture(0, vrController::ray_baked->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, ray_baked_screen->GLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_con_size", vrController::_screen_w, vrController::_screen_h);
    Shader::Uniform(sp, "u_fov", vrController::camera->getFOV());

    glm::mat4 model_inv = glm::inverse(vrController::ModelMat_ * glm::scale(glm::mat4(1.0), glm::vec3(0.75f)));
    Shader::Uniform(sp, "u_WorldToModel", model_inv);
    Shader::Uniform(sp, "u_CamToWorld", glm::translate(glm::mat4(1.0), vrController::camera->getCameraPosition()));
    Shader::Uniform(sp, "uCamposObjSpace", glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
    Shader::Uniform(sp, "uViewDir", vrController::camera->getViewDirection().z);
    Shader::Uniform(sp,"usample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
    cutter_->Update();
    cutter_->setCuttingParams(sp);

    glDispatchCompute((GLuint)(ray_baked_screen->Width() + 7) / 8, (GLuint)(ray_baked_screen->Height() + 7) / 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    cshader_->UnUse();
    baked_dirty_ = false;
}

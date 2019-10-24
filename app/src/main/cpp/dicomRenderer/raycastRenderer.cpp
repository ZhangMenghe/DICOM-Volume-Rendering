#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include "raycastRenderer.h"
#include <GLPipeline/Primitive.h>
#include <AndroidUtils/mathUtils.h>
raycastRenderer::raycastRenderer() {
    //geometry
    Mesh::InitQuadWithTex(VAO_, cuboid_with_texture, 8, cuboid_indices, 36);

    //program
    shader_ = new Shader();
    if(!shader_->Create("shaders/raycastVolume.vert", "shaders/raycastVolume.frag"))
        LOGE("Raycast===Failed to create shader program===");
    //geometry program
    geoshader_ = new Shader;
    if(!geoshader_->Create("shaders/raycastVolume.glsl"))
        LOGE("Raycast=====Failed to create geometry shader");

    cutter_ = new cuttingController;//(glm::vec3(.0f), glm::vec3(0,0,-1));
}
void raycastRenderer::Draw(){
    precompute();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    //Update cutting plane and draw
    cutter_->Draw();

    shader_->Use();
    if(vrController::tex_baked){
        glActiveTexture(GL_TEXTURE0+vrController::BAKED_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::tex_baked->GLTexture());
        shader_->setInt("uSampler_tex", vrController::BAKED_TEX_ID);
    }else{
        glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());
        shader_->setInt("uSampler_tex", vrController::VOLUME_TEX_ID);
    }

        shader_->setMat4("uProjMat", vrController::camera->getProjMat());
        shader_->setMat4("uViewMat", vrController::camera->getViewMat());
        shader_->setMat4("uModelMat", vrController::ModelMat_);

        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
        shader_->setVec3("uCamposObjSpace", glm::vec3(model_inv
        *glm::vec4(vrController::camera->getCameraPosition(), 1.0)));
        shader_->setVec3("uVolumeSize",
                glm::vec3(vrController::tex_volume->Width(),
                vrController::tex_volume->Height(),
                vrController::tex_volume->Depth()));

        shader_->setBool("ub_colortrans", vrController::param_bool_map["colortrans"]);
        shader_->setBool("ub_accumulate", vrController::param_bool_map["accumulate"]);
        shader_->setBool("ub_cuttingplane", vrController::param_bool_map["cutting"]);

        shader_->setFloat("sample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
        shader_->setFloat("val_threshold", vrController::param_value_map["threshold"]);
        shader_->setFloat("brightness", vrController::param_value_map["brightness"]);

        cutter_->setCuttingParams(shader_);

    if(vrController::camera->getViewDirection().z <0)
        glFrontFace(GL_CW);
    else
        glFrontFace(GL_CCW);

        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    shader_->unUse();


    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}

void raycastRenderer::onCuttingChange(float percent){
    cutter_->setCutPlane(percent);
}
void raycastRenderer::precompute() {
    Texture* tex_vol = vrController::tex_volume;
    if(baked_dirty_) {

        geoshader_->Use();

        glBindImageTexture(0, tex_vol->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
        glBindImageTexture(1, vrController::tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

        glDispatchCompute((GLuint)(tex_vol->Width() + 7) / 8, (GLuint)(tex_vol->Height() + 7) / 8, (GLuint)(tex_vol->Depth() + 7) / 8);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
        glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

        //bool type not working
//        shader_->setBool("ub_maskon", vrController::param_bool_map["maskon"]);
//        shader_->setBool("ub_maskonly", true);//vrController::param_bool_map["maskon"]);
//        shader_->setBool("ub_invert", true);//vrController::param_bool_map["maskon"]);

        geoshader_->unUse();
        baked_dirty_ = false;
    }
}

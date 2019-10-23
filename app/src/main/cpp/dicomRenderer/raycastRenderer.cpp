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
    shader_->Use();
        shader_->setInt("uSampler_tex", vrController::VOLUME_TEX_ID);
//        shader_->setInt("uSampler_trans", vrController::TRANS_TEX_ID);
        shader_->setVec3("uVolumeSize", glm::vec3(vrController::tex_volume->Width(), vrController::tex_volume->Height(), vrController::tex_volume->Depth()));
    shader_->unUse();

    //geometry program
    geoshader_ = new Shader;
    if(!geoshader_->Create("shaders/raycastVolume.glsl"))
        LOGE("Raycast=====Failed to create geometry shader");
    cutter_ = new cuttingController;//(glm::vec3(.0f), glm::vec3(0,0,-1));
}
void raycastRenderer::Draw(){
    precompute();
    // precompute here, update texture

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, bake_tex_->GLTexture());

    //Update cutting plane and draw
    cutter_->Draw();

    shader_->Use();
        shader_->setMat4("uProjMat", vrController::camera->getProjMat());
        shader_->setMat4("uViewMat", vrController::camera->getViewMat());
        shader_->setMat4("uModelMat", vrController::ModelMat_);

        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
        shader_->setVec3("uCamposObjSpace", glm::vec3(model_inv
        *glm::vec4(vrController::camera->getCameraPosition(), 1.0)));

        shader_->setBool("ub_colortrans", vrController::param_bool_map["colortrans"]);
        shader_->setBool("ub_accumulate", vrController::param_bool_map["accumulate"]);
        shader_->setBool("ub_cuttingplane", vrController::param_bool_map["cutting"]);
        shader_->setBool("ub_colononly", vrController::param_bool_map["maskon"]);

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
        if(!bake_tex_){
            GLbyte * data = new GLbyte[tex_vol->Width() * tex_vol->Height() * tex_vol->Depth() * 4];
            memset(data, 0xff, tex_vol->Width() * tex_vol->Height() * tex_vol->Depth() * 4 * sizeof(GLbyte));
            bake_tex_ = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, tex_vol->Width(), tex_vol->Height(), tex_vol->Depth(), data);
        }

        glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, bake_tex_->GLTexture());
        geoshader_->Use();

        glBindImageTexture(0, bake_tex_->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glDispatchCompute((GLuint)(bake_tex_->Width() + 7) / 8, (GLuint)(bake_tex_->Height() + 7) / 8, (GLuint)(bake_tex_->Depth() + 7) / 8);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

        geoshader_->unUse();
        baked_dirty_ = false;
    }
}

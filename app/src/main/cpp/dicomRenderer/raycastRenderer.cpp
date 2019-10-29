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
    if(!shader_->AddShaderFile(GL_VERTEX_SHADER,"shaders/raycastVolume.vert")
            ||!shader_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/raycastVolume.frag")
            ||!shader_->CompileAndLink())
        LOGE("Raycast===Failed to create raycast shader program===");
    cutter_ = new cuttingController;//(glm::vec3(.0f), glm::vec3(0,0,-1));
}
void raycastRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    //Update cutting plane and draw
    cutter_->Draw();

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

//    Shader::Uniform(sp,"ub_colortrans", vrController::param_bool_map["colortrans"]);
    Shader::Uniform(sp,"ub_accumulate", vrController::param_bool_map["accumulate"]);
    Shader::Uniform(sp,"ub_cuttingplane", vrController::param_bool_map["cutting"]);

    Shader::Uniform(sp,"sample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
    Shader::Uniform(sp,"val_threshold", vrController::param_value_map["threshold"]);
    Shader::Uniform(sp,"brightness", vrController::param_value_map["brightness"]);

        cutter_->setCuttingParams(shader_);

    if(vrController::camera->getViewDirection().z <0)
        glFrontFace(GL_CW);
    else
        glFrontFace(GL_CCW);

        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    shader_->UnUse();


    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}

void raycastRenderer::onCuttingChange(float percent){
    cutter_->setCutPlane(percent);
}

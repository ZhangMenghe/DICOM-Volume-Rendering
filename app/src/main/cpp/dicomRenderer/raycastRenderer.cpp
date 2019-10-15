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
    cutter_ = new cuttingController;//(glm::vec3(.0f), glm::vec3(0,0,-1));
//            vec3MatNorm(glm::inverse(vrController::RotateMat_),
//            vrController::camera->getViewDirection()));
}
void raycastRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());

//    glActiveTexture(GL_TEXTURE0 + vrController::TRANS_TEX_ID);
//    glBindTexture(GL_TEXTURE_2D, vrController::tex_trans->GLTexture());

    shader_->Use();
        shader_->setMat4("uProjMat", vrController::camera->getProjMat());
        shader_->setMat4("uViewMat", vrController::camera->getViewMat());
        shader_->setMat4("uModelMat", vrController::ModelMat_);

        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
        shader_->setVec3("uCamposObjSpace", glm::vec3(model_inv
        *glm::vec4(vrController::camera->getCameraPosition(), 1.0)));


        shader_->setBool("ub_simplecube", vrController::param_bool_map["simplecube"]);
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
    cutter_->Draw();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}

void raycastRenderer::onCuttingChange(float percent){
    cutter_->setCutPlane(percent);
//    cplane_point = cplane_start_ + glm::normalize(cplane_normal) * percent * 1.75f;
//    return;
//    //if view direction change
//    if(!vrController::ROTATE_AROUND_CUBE && vrController::view_dirDirty){
//        vrController::view_dirDirty = false;
//        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
//        glm::vec3 p= glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0));
//        glm::vec3 pn = -p;
//        float d = -(pn.x * p.x + pn.y * p.y + pn.z * p.z);
//
//        float shortest_dist = FLT_MAX; int vertex_idx;
//        for(int i=0; i<8; i++){
//            float dist = shortest_distance(cuboid_with_texture[6*i], cuboid_with_texture[6*i+1], cuboid_with_texture[6*i+2], pn.x, pn.y, pn.z, d);
//            if(dist < shortest_dist){
//                shortest_dist = dist;
//                vertex_idx = i;
//            }
//        }
//        shader_->Use();
//        shader_->setVec3("uStartPoint", cuboid_with_texture[6*vertex_idx], cuboid_with_texture[6*vertex_idx + 1], cuboid_with_texture[6*vertex_idx + 2]);
//    }
//
//    shader_->Use();//cutting in obj space
//    shader_->setFloat("cut_percent", percent);

}

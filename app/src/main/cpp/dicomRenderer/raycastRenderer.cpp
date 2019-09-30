#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include "raycastRenderer.h"
raycastRenderer::raycastRenderer() {
    //geometry
    Mesh::InitQuadWithTex(VAO_, sVertex, 8, sIndices, 36);

    //program
    shader_ = new Shader();
    if(!shader_->Create("shaders/raycastVolume.vert", "shaders/raycastVolume.frag"))
        LOGE("Raycast===Failed to create shader program===");
    shader_->Use();
        shader_->setInt("uSampler_tex", vrController::VOLUME_TEX_ID);
//        shader_->setInt("uSampler_trans", vrController::TRANS_TEX_ID);
        shader_->setVec3("uVolumeSize", glm::vec3(vrController::tex_volume->Width(), vrController::tex_volume->Height(), vrController::tex_volume->Depth()));
    shader_->unUse();
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
//        shader_->setVec3("uPlane.p", glm::vec3(model_inv * glm::vec4(vrController::cplane_p, 1.0)));
//        shader_->setVec3("uPlane.normal", vrController::cplane_normal);
        shader_->setBool("uPlane.upwards", true);

        shader_->setVec3("uSphere.center", glm::vec3(model_inv * glm::vec4(vrController::csphere_c, 1.0)));
        shader_->setFloat("uSphere.radius", vrController::csphere_radius);
        shader_->setBool("uSphere.outside", true);

        shader_->setBool("ub_simplecube", vrController::param_bool_map["simplecube"]);
        shader_->setBool("ub_colortrans", vrController::param_bool_map["colortrans"]);
        shader_->setFloat("sample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
        shader_->setFloat("val_threshold", vrController::param_value_map["threshold"]);
        shader_->setFloat("brightness", vrController::param_value_map["brightness"]);

        shader_->setFloat("uOpacitys.overall", vrController::param_value_map["overall"]);
        shader_->setFloat("uOpacitys.lowbound", vrController::param_value_map["lowbound"]);
        shader_->setFloat("uOpacitys.cutoff", vrController::param_value_map["cutoff"]);

    if(vrController::camera->getViewDirection().z <=0)
        glFrontFace(GL_CCW);
    else
        glFrontFace(GL_CW);

        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    shader_->unUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}
void raycastRenderer::onCuttingChange(float percent){
    glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);

//    vrController::cplane_p = glm::vec3(.0f);
    //get the nearest - farest vertex
    //s1. trans eye position back to model-coord
    glm::vec3 eye_model3 = glm::vec3( model_inv * glm::vec4(vrController::camera->getCameraPosition(), 1.0)) ;
    glm::vec3 vdir_model = -glm::normalize(eye_model3);
    LOGE("=====eye pos: %f, %f, %f, dir %f %f %f", eye_model3.x, eye_model3.y, eye_model3.z, vdir_model.x, vdir_model.y, vdir_model.z);

//    if(!glm::any(glm::equal(vrController::camera->getViewDirection(), last_cutting_norm))){

    //update cutting nearest - fartest position
//    glm::vec3 nearest_plane_intersection;
//    if(!RayAABBIntersection(eye_model3, vdir_model,
//                                     glm::vec3(-0.5), glm::vec3(0.5),
//                                     nearest_plane_intersection)) return; //not cutting

//    float hdist = glm::distance(vrController::camera->getViewCenter() , nearest_plane_intersection);
//    hdist*=1.3f;
//    glm::vec3 start_cutting = -hdist * vdir_model;
//    float cutting_length = 2.0f * hdist;
//    glm::vec3 pop_model = start_cutting + percent * cutting_length * vdir_model;

    vdir_model = glm::vec3(.0f, .0f, -1.0f);
    vrController::cplane_p += 0.01f * vdir_model;
    shader_->Use();//cutting in obj space
    shader_->setVec3("uPlane.p", glm::vec3(model_inv * glm::vec4(vrController::cplane_p, 1.0)));
    LOGE("===cutting plane point: %f, %f, %f", vrController::cplane_p.x, vrController::cplane_p.y, vrController::cplane_p.z);//, vdir_model.x, vdir_model.y, vdir_model.z);

    shader_->setVec3("uPlane.normal", vdir_model);
}
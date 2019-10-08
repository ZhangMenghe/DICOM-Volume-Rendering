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

        shader_->setVec3("uSphere.center", glm::vec3(vrController::csphere_c));
        shader_->setFloat("uSphere.radius", vrController::csphere_radius);

        shader_->setBool("ub_simplecube", vrController::param_bool_map["simplecube"]);

        shader_->setBool("ub_colortrans", vrController::param_bool_map["colortrans"]);
        shader_->setBool("ub_accumulate", vrController::param_bool_map["accumulate"]);


        shader_->setFloat("sample_step_inverse", 1.0f / vrController::param_value_map["samplestep"]);
        shader_->setFloat("val_threshold", vrController::param_value_map["threshold"]);
        shader_->setFloat("brightness", vrController::param_value_map["brightness"]);

        shader_->setFloat("uOpacitys.overall", vrController::param_value_map["overall"]);
        shader_->setFloat("uOpacitys.lowbound", vrController::param_value_map["lowbound"]);
        shader_->setFloat("uOpacitys.cutoff", vrController::param_value_map["cutoff"]);

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
float shortest_distance(float x1, float y1,
                       float z1, float a,
                       float b, float c,
                       float d){
    d = fabs((a * x1 + b * y1 +
              c * z1 + d));
    float e = sqrt(a * a + b *
                           b + c * c);
    return d/e;
}
void raycastRenderer::onCuttingChange(float percent){
    return;
    //if view direction change
    if(vrController::view_dirDirty){
        vrController::view_dirDirty = false;
        glm::mat4 model_inv = glm::inverse(vrController::ModelMat_);
        glm::vec3 p= glm::vec3(model_inv*glm::vec4(vrController::camera->getCameraPosition(), 1.0));
        glm::vec3 pn = -p;
        float d = -(pn.x * p.x + pn.y * p.y + pn.z * p.z);

        float shortest_dist = FLT_MAX; int vertex_idx;
        for(int i=0; i<8; i++){
            float dist = shortest_distance(sVertex[6*i], sVertex[6*i+1], sVertex[6*i+2], pn.x, pn.y, pn.z, d);
            if(dist < shortest_dist){
                shortest_dist = dist;
                vertex_idx = i;
            }
        }
        shader_->Use();
        shader_->setVec3("uStartPoint", sVertex[6*vertex_idx], sVertex[6*vertex_idx + 1], sVertex[6*vertex_idx + 2]);
    }

    shader_->Use();//cutting in obj space
    shader_->setFloat("cut_percent", percent);
}
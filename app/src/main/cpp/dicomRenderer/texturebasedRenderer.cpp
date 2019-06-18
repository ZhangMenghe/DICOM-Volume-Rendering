#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include "texturebasedRenderer.h"
texvrRenderer::texvrRenderer() {
    modelMat_ = glm::mat4(1.0);
    //geometry
    size_t dims = NEED_SLCIES;
    m_VAOs = std::vector<GLuint >(dims);
    float dimension_inv = 1.0f / dims;

    float vertices[24];
    float mappedZVal = -scale_inv, zTex = .0f;
    for (int i = 0; i < dims; i++){
        memcpy(vertices, mVertices, sizeof(float)*24);
        for(int j=0;j<4;j++){vertices[6*j + 2] = mappedZVal;vertices[6*j + 5] = zTex; }
        mappedZVal+=2.0 * dimension_inv* scale_inv; zTex+=dimension_inv;

        Mesh::InitQuadWithTex(m_VAOs[i], vertices, 4, indices, 6);
    }

    //program
    shader_ = new Shader();
    if(!shader_->Create("shaders/textureVolume.vert", "shaders/textureVolume.frag"))
        LOGE("TextureBas===Failed to create shader program===");
    shader_->Use();
        shader_->setInt("uSampler_tex", vrController::VOLUME_TEX_ID);
        shader_->setInt("uSampler_trans", vrController::TRANS_TEX_ID);
        shader_->setVec3("uVolumeSize", glm::vec3(vrController::tex_volume->Width(), vrController::tex_volume->Height(), vrController::tex_volume->Depth()));

        shader_->setBool("u_use_color_transfer", vrController::b_use_color_transfer);
        shader_->setFloat("uOpacitys.overall", vrController::opa_oa);
        shader_->setFloat("uOpacitys.low_limit", vrController::opa_ll);
        shader_->setFloat("uOpacitys.cut_off", vrController::opa_co);
    shader_->unUse();

}
void texvrRenderer::Draw(){

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0+vrController::VOLUME_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());

    glActiveTexture(GL_TEXTURE0 + vrController::TRANS_TEX_ID);
    glBindTexture(GL_TEXTURE_2D, vrController::tex_trans->GLTexture());

    shader_->Use();
        shader_->setMat4("uProjMat", vrController::camera->getProjMat());
        shader_->setMat4("uViewMat", vrController::camera->getViewMat());
        shader_->setMat4("uModelMat", modelMat_);


        if(vrController::camera->getViewDirection().z <0){
            glFrontFace(GL_CW);
            for(int id = 0; id <m_VAOs.size()- slice_start_idx; id++){
                glBindVertexArray(m_VAOs[id]);glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        else{
            glFrontFace(GL_CCW);
            for (int id = m_VAOs.size()-1; id >= slice_start_idx; id--) {
                glBindVertexArray(m_VAOs[id]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    shader_->unUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
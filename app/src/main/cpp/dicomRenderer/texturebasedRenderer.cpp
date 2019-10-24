#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include "texturebasedRenderer.h"
texvrRenderer::texvrRenderer() {
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
    if(!shader_->AddShaderFile(GL_VERTEX_SHADER,"shaders/textureVolume.vert")
       ||!shader_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/textureVolume.frag")
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create raycast shader program===");
}
void texvrRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    GLuint sp = shader_->Use();
    if(vrController::tex_baked){
        glActiveTexture(GL_TEXTURE0 + vrController::BAKED_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::tex_baked->GLTexture());
        Shader::Uniform(sp, "uSampler_tex", vrController::BAKED_TEX_ID);
    }else{
        glActiveTexture(GL_TEXTURE0 + vrController::VOLUME_TEX_ID);
        glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());
        Shader::Uniform(sp, "uSampler_tex", vrController::VOLUME_TEX_ID);
    }

    Shader::Uniform(sp,"uVolumeSize",
                    glm::vec3(vrController::tex_volume->Width(),
                              vrController::tex_volume->Height(),
                              vrController::tex_volume->Depth()));
    Shader::Uniform(sp,"uProjMat", vrController::camera->getProjMat());
    Shader::Uniform(sp,"uViewMat", vrController::camera->getViewMat());
        if(vrController::ROTATE_AROUND_CUBE)
            Shader::Uniform(sp,"uModelMat", glm::mat4(1.0));
        else Shader::Uniform(sp,"uModelMat", vrController::ModelMat_);

    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_value_map["overall"]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_value_map["lowbound"]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_value_map["cutoff"]);

        bool clock_wise = false;
        if(vrController::ROTATE_AROUND_CUBE){
            if(vrController::camera->getViewDirection().z <0){
                glFrontFace(GL_CW);
                for(int id = 0; id <m_VAOs.size()- slice_start_idx; id++){
                    glBindVertexArray(m_VAOs[id]);glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }else{
                glFrontFace(GL_CCW);
                for (int id = m_VAOs.size()-1; id >= slice_start_idx; id--) {
                    glBindVertexArray(m_VAOs[id]);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }
        }



        else{
            glm::vec3 dir = glm::vec3(vrController::RotateMat_ * glm::vec4(.0,.0,-1.0,1.0));
            if(dir.z < 0){
                glFrontFace(GL_CW);
            }else{
                glFrontFace(GL_CCW);
            }
            for(int id = 0; id <m_VAOs.size()- slice_start_idx; id++)
            {glBindVertexArray(m_VAOs[id]);glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);}
        }

//        if(clock_wise){
//            glFrontFace(GL_CW);
//            for(int id = 0; id <m_VAOs.size()- slice_start_idx; id++){
//                glBindVertexArray(m_VAOs[id]);glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//            }
//        }
//        else{
//            glFrontFace(GL_CCW);
//            for (int id = m_VAOs.size()-1; id >= slice_start_idx; id--) {
//                glBindVertexArray(m_VAOs[id]);
//                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//            }
//        }
    shader_->UnUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
void texvrRenderer::onCuttingChange(float percent){
    slice_start_idx = (int)(m_VAOs.size() * percent);
}
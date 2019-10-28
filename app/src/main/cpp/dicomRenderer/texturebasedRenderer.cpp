#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
texvrRenderer::texvrRenderer() {
    //geometry
    size_t dims = NEED_SLCIES;
//    m_VAOs = std::vector<GLuint >(dims);
    float dimension_inv = 1.0f / dims;
//
//    float vertices[24];
//    float mappedZVal = -scale_inv, zTex = .0f;
//    for (int i = 0; i < dims; i++){
//        memcpy(vertices, mVertices, sizeof(float)*24);
//        for(int j=0;j<4;j++){vertices[6*j + 2] = mappedZVal;vertices[6*j + 5] = zTex; }
//        mappedZVal+=2.0 * dimension_inv* scale_inv; zTex+=dimension_inv;
//
//        Mesh::InitQuadWithTex(m_VAOs[i], vertices, 4, indices, 6);
//    }

    glm::vec2 *zInfos = new glm::vec2[dims];

    float mappedZVal = -scale_inv, zTex = .0f;
    for (int i = 0; i < dims; i++){
        zInfos[i].x = mappedZVal; zInfos[i].y = zTex;
        mappedZVal+=2.0 * dimension_inv* scale_inv; zTex+=dimension_inv;
    }

    // store instance data in an array buffer
    // --------------------------------------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * NEED_SLCIES, zInfos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);





    glGenVertexArrays(1, &slice_vao_);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)slice_vao_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, quad_vertices_2d, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, quad_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.

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
        Shader::Uniform(sp, "uSampler_baked", vrController::BAKED_TEX_ID);
    }
    glActiveTexture(GL_TEXTURE0 + vrController::VOLUME_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::tex_volume->GLTexture());
    Shader::Uniform(sp, "uSampler_tex", vrController::VOLUME_TEX_ID);
    if(vrController::ROTATE_AROUND_CUBE)
        Shader::Uniform(sp,"uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat());
    else
        Shader::Uniform(sp,"uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat()*vrController::ModelMat_);

    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_value_map["overall"]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_value_map["lowbound"]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_value_map["cutoff"]);

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
        if(dir.z < 0) glFrontFace(GL_CCW);
        else  glFrontFace(GL_CW);
        glBindVertexArray(slice_vao_); glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, NEED_SLCIES);
//        for(int id = 0; id <m_VAOs.size()- slice_start_idx; id++)
//        {glBindVertexArray(m_VAOs[id]);glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);}
    }
    shader_->UnUse();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
void texvrRenderer::onCuttingChange(float percent){
    slice_start_idx = (int)(m_VAOs.size() * percent);
}
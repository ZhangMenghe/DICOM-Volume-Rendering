#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "backgroundRenderer.h"
#include <GLES2/gl2ext.h>
namespace {
// Positions of the quad vertices in clip space (X, Y).
    const GLfloat kVertices[] = {
            -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
    };
}
backgroundRenderer::backgroundRenderer(){
//    const unsigned int bg_indices[6]= {0,1,2,
//                                         0,2,4};
//    Mesh::InitQuadWithTex(vao_, vbo_, 4, bg_indices, 6);

    Mesh::InitQuadWithTex(vao_, quad_vertices_tex_standard, 4, quad_indices, 6);

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    TEX_ID = vrController::BAKED_RAY_ID + 2;

    if(!shader_.AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_AR_BACKGROUND_SCREEN_VERT])
       ||!shader_.AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_AR_BACKGROUND_SCREEN_FRAG])
       ||!shader_.CompileAndLink())
        LOGE("ar-background===Failed to create opacity shader program===");
}
void backgroundRenderer::Draw(float * uvs_){
//    LOGE("====%p", this);
    //update vertices
//    const float test[8]= {
//            0.0f,0.0f,1.0f,0.0f, 0.0f,1.0f,1.0f,1.0f
//    };
//    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
//    glBufferSubData(GL_ARRAY_BUFFER, 0,  8  *sizeof(float), kVertices);
//    glBufferSubData(GL_ARRAY_BUFFER, 8  *sizeof(float), 8 *sizeof(float), test);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint sp = shader_.Use();
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0+TEX_ID);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id_);
    Shader::Uniform(sp, "uSampler", TEX_ID);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
}
GLuint backgroundRenderer::GetTextureId() const{ return texture_id_; }
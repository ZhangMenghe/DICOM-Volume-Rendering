#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "screenQuad.h"
screenQuad* screenQuad::myPtr_ = nullptr;
screenQuad* screenQuad::instance(){
    if(!myPtr_) myPtr_ = new screenQuad();
    return myPtr_;
}

screenQuad::screenQuad(){
    Mesh::InitQuadWithTex(vao_, quad_vertices_tex_standard, 4, quad_indices, 6);

    float width = vrController::_screen_w, height = vrController::_screen_h;
    if(height > TEX_HEIGHT){
        tex_width = width / height * TEX_HEIGHT; tex_height = TEX_HEIGHT;
    }else{
        tex_width = width; tex_height = height;
    }
    int vsize= tex_width* tex_height;
    GLbyte * vdata = new GLbyte[vsize * 4];
    memset(vdata, 0x00, vsize * 4 * sizeof(GLbyte));
    qtex_ = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, tex_width, tex_height, vdata);
    TEX_ID = vrController::BAKED_RAY_ID + 1;

    if(!qshader_.AddShader(GL_VERTEX_SHADER,vrController::shader_contents[dvr::SHADER_QUAD_VERT])
       ||!qshader_.AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_QUAD_FRAG])
       ||!qshader_.CompileAndLink())
        LOGE("Screen===Failed to create screen shader program===");
    myPtr_ = this;
}

void screenQuad::Draw(){
    glViewport(0, 0, vrController::_screen_w, vrController::_screen_h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render to screen
    GLuint sp = qshader_.Use();
    glActiveTexture(GL_TEXTURE0+TEX_ID);
    glBindTexture(GL_TEXTURE_2D, qtex_->GLTexture());
    Shader::Uniform(sp, "uSampler", TEX_ID);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    qshader_.UnUse();
}
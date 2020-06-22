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
    if(!qshader_.AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_QUAD_VERT])
       ||!qshader_.AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_QUAD_FRAG])
       ||!qshader_.CompileAndLink())
        LOGE("Screen===Failed to create screen shader program===\n");
    Manager::shader_contents[dvr::SHADER_QUAD_VERT] = "";Manager::shader_contents[dvr::SHADER_QUAD_FRAG]="";
    myPtr_ = this;
}
void screenQuad::onScreenSizeChange(float width, float height){
    if(width == tex_width && tex_height == height) return;

    tex_width = GLuint (width);
    tex_height = GLuint(height);
    auto vsize = tex_width* tex_height * 4;
    GLbyte * vdata = new GLbyte[vsize];
    memset(vdata, 0x00, vsize * sizeof(GLbyte));
    if(qtex_) delete qtex_;

    qtex_ = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, tex_width, tex_height, vdata);
}
void screenQuad::Draw(){
    glViewport(0, 0, tex_width, tex_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render to screen
    GLuint sp = qshader_.Use();
    glActiveTexture(GL_TEXTURE0 + dvr::SCREEN_QUAD_TEX_ID);
    glBindTexture(GL_TEXTURE_2D, qtex_->GLTexture());
    Shader::Uniform(sp, "uSampler", dvr::SCREEN_QUAD_TEX_ID);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    qshader_.UnUse();
}

void screenQuad::Clear(){
    if(!frame_buff_) Texture::initFBO(frame_buff_, qtex_, nullptr);
    glViewport(0, 0, tex_width, tex_height);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "backgroundRenderer.h"
#include <GLES2/gl2ext.h>
#include <dicomRenderer/screenQuad.h>

backgroundRenderer::backgroundRenderer(bool screen_baked)
:DRAW_TO_TEXTURE(screen_baked){
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,  8  *sizeof(float), kVertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!shader_.AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_AR_BACKGROUND_SCREEN_VERT])
       ||!shader_.AddShader(GL_FRAGMENT_SHADER,  vrController::shader_contents[dvr::SHADER_AR_BACKGROUND_SCREEN_FRAG])
       ||!shader_.CompileAndLink())
        LOGE("ar-background===Failed to create opacity shader program===");
}
void backgroundRenderer::Draw(float * uvs_){
    //update uv data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 8*sizeof(float), 8 *sizeof(float), uvs_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(DRAW_TO_TEXTURE) draw_to_texture();
    else draw_scene();
}
void backgroundRenderer::draw_to_texture(){
    if(!baked_dirty_) return;

    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}
void backgroundRenderer::draw_scene(){
    GLuint sp = shader_.Use();
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0+dvr::BACK_GROUND_AR_ID);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id_);
    Shader::Uniform(sp, "uSampler", dvr::BACK_GROUND_AR_ID);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    shader_.UnUse();
    glDepthMask(GL_TRUE);
}
GLuint backgroundRenderer::GetTextureId() const{ return texture_id_; }
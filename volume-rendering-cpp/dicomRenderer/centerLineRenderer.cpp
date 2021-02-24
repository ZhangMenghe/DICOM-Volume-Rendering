#include "centerLineRenderer.h"
#include <Manager.h>
#include <dicomRenderer/Constants.h>
#include <dicomRenderer/screenQuad.h>
#include <glm/gtx/string_cast.hpp>
centerLineRenderer::centerLineRenderer(int id):uid(id){
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_POINT_NUM * 3, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if(!shader_.AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_CPLANE_VERT])
       ||!shader_.AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_CPLANE_FRAG])
       ||!shader_.CompileAndLink())
        LOGE("Center Line===Failed to create shader program===");
    GLuint sp = shader_.Use();

    Shader::Uniform(sp, "uBaseColor", (id==4)?glm::vec4(1.0f, .0f, .0f, 1.0f):glm::vec4(1.0f, 1.0f, .0f, 1.0f));
    shader_.UnUse();
    baked_dirty_ = true;
}
void centerLineRenderer::updateVertices(int point_num, const float * data){
    draw_point_num = point_num;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, point_num * 3 * sizeof(float), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    baked_dirty_ = true;
}
void centerLineRenderer::onDraw(bool pre_draw, glm::mat4 model_mat){
    if(pre_draw) draw_baked(model_mat);
    else draw_scene(model_mat);
}
void centerLineRenderer::draw_scene(glm::mat4 model_mat){   
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uMVP", Manager::camera->getVPMat(dvr::AR_USE_MARKER)*model_mat);
    glBindVertexArray(vao_);

    glDrawArrays(GL_LINE_STRIP, 0, draw_point_num);
    glBindVertexArray(0);
    shader_.UnUse();
}
void centerLineRenderer::draw_baked(glm::mat4 model_mat){
//    if(!baked_dirty_) return;
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(model_mat);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}

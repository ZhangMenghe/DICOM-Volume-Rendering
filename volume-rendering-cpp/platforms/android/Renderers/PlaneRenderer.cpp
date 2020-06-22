#include <vrController.h>
#include <dicomRenderer/screenQuad.h>
#include <platforms/android/Utils/assetLoader.h>
#include "PlaneRenderer.h"
PlaneRenderer::PlaneRenderer(bool screen_baked)
    :DRAW_TO_TEXTURE(screen_baked){

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1,&ibo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAX_PLANE_VERTICES * 3, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * MAX_PLANE_VERTICES*5, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (!assetLoader::instance()->LoadPngFromAssetManager(GL_TEXTURE_2D, "models/trigrid.png")) {
        LOGE("===Could not load png texture for planes.");
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(!shader_.AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_AR_PLANE_VERT])
       ||!shader_.AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_AR_PLANE_FRAG])
       ||!shader_.CompileAndLink())
        LOGE("PLANE===Failed to create shader program===");
}
void PlaneRenderer::Draw(std::vector<glm::vec3> vertices, std::vector<GLushort> indices,
          glm::mat4 viewproj_mat, glm::mat4 model_mat, glm::vec3 normal_vec,
          glm::vec3 color){
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size()*3* sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), indices.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if(DRAW_TO_TEXTURE) draw_to_texture(indices, viewproj_mat*model_mat, model_mat, normal_vec, color);
    else draw_scene(indices, viewproj_mat*model_mat, model_mat, normal_vec, color);
}
void PlaneRenderer::draw_to_texture(std::vector<GLushort> indices,
                                    glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec,
                                    glm::vec3 color){
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(indices, mvp, model_mat, normal_vec, color);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void PlaneRenderer::draw_scene(std::vector<GLushort> indices,
        glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec,
        glm::vec3 color){
    GLuint sp = shader_.Use();
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0 + dvr::PLANE_AR_ID);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    Shader::Uniform(sp, "uMVP", mvp);
    Shader::Uniform(sp, "uModelMat", model_mat);
    Shader::Uniform(sp, "uNormal", normal_vec);
    Shader::Uniform(sp, "uSampler", dvr::PLANE_AR_ID);
    Shader::Uniform(sp, "uColor", color);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                   indices.data());

    glBindVertexArray(0);
    shader_.UnUse();
    glDepthMask(GL_TRUE);
}
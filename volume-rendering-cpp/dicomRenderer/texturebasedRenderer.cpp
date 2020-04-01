#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
#include "screenQuad.h"

texvrRenderer::texvrRenderer(bool screen_baked)
        :DRAW_BAKED(screen_baked){
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER, vrController::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER, vrController::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create texture based shader program===");
    vrController::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT] = "";vrController::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG]="";

    onCuttingChange(.0f);
}

void texvrRenderer::init_vertices(){
    glGenBuffers(1, &vbo_instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_DIMENSIONS, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_slice);
    unsigned int VBO, ibo;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &ibo);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_slice);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, quad_vertices_2d, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, quad_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.
    b_init_successful = true;

    update_instance_data();
}
void texvrRenderer::update_instance_data(){
    if(dimensions == 0) return;
    glm::vec2 *zInfos = new glm::vec2[dimensions];

    float zTex = .0f;
    float mappedZVal = -scale_inv + 0.5f* (MAX_DIMENSIONS - dimensions)/MAX_DIMENSIONS;
    for (int i = 0; i < dimensions; i++){
        zInfos[i].x = mappedZVal; zInfos[i].y = zTex;
        mappedZVal+=MAX_DIMENSIONS_INV; zTex+=dimension_inv;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dimensions *sizeof(glm::vec2), zInfos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[]zInfos;
}
//BE SUPER CAUTIOUS TO CHANGE!
void texvrRenderer::draw_scene(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
  
    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
  
    Shader::Uniform(sp, "uSampler_baked", dvr::BAKED_TEX_ID);

    glm::mat4 modelmat = vrController::instance()->getModelMatrix();
    Shader::Uniform(sp, "uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat() * modelmat);

    //for backface rendering! don't erase
    glm::mat4 rotmat = vrController::instance()->getRotationMatrix();
//    glm::vec3 dir = glm::vec3(rotmat[0][2], rotmat[1][2],rotmat[2][2]);
//    if(glm::dot(vrController::camera->getViewDirection(), dir) < 0) glFrontFace(GL_CCW);
    if(rotmat[2][2] > 0) glFrontFace(GL_CCW);
    else glFrontFace(GL_CW);
    glBindVertexArray(vao_slice); glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, dimensions);

    shader_->UnUse();
    glFrontFace(GL_CCW);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void texvrRenderer::Draw(){
    if(!b_init_successful) init_vertices();
    if(DRAW_BAKED || vrController::param_bool[dvr::CHECK_ARENABLED]) draw_baked();
    else draw_scene();
}

void texvrRenderer::onCuttingChange(float percent){
    int cut_id = int(dimensions * percent);
    GLuint sp = shader_->Use();
    Shader::Uniform(sp, "u_cut_texz", 1.0f-dimension_inv * cut_id);
    shader_->UnUse();
}

void texvrRenderer::updatePrecomputation(GLuint sp) {
    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_tex[dvr::TT_OVERALL]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_tex[dvr::TT_LOWEST]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_tex[dvr::TT_CUTOFF]);
}

void texvrRenderer::draw_baked() {
    if(!baked_dirty_) return;
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}

void texvrRenderer::setDimension(int dims){
    dimensions = int(dims * DENSE_FACTOR);dimension_inv = 1.0f / dimensions;
    update_instance_data();
}

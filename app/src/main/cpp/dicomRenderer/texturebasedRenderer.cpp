#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
#include "screenQuad.h"

texvrRenderer::texvrRenderer(bool screen_baked)
:DRAW_BAKED(screen_baked){
    //program
    shader_ = new Shader();
    if(!shader_->AddShaderFile(GL_VERTEX_SHADER,"shaders/textureVolume.vert")
       ||!shader_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/textureVolume.frag")
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create texture based shader program===");
    onCuttingChange(.0f);
}
void texvrRenderer::init_vertices(){
    dimensions = int(vrController::VOL_DIMS.z * DENSE_FACTOR);
    if(dimensions == 0) return;

    dimension_inv = 1.0f / dimensions;
    glm::vec2 *zInfos = new glm::vec2[dimensions];

    float mappedZVal = -scale_inv, zTex = .0f;
    for (int i = 0; i < dimensions; i++){
        zInfos[i].x = mappedZVal; zInfos[i].y = zTex;
        mappedZVal+=2.0 * dimension_inv* scale_inv; zTex+=dimension_inv;
    }

    // store instance data in an array buffer
    // --------------------------------------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * dimensions, zInfos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_slice);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_slice);

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
    b_init_successful = true;
}
void texvrRenderer::draw_scene(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!vrController::param_bool[dvr::CHECK_MASKON]){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
    }
    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + vrController::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::tex_baked->GLTexture());
    Shader::Uniform(sp, "uSampler_baked", vrController::BAKED_TEX_ID);

    if(vrController::ROTATE_AROUND_CUBE)
        Shader::Uniform(sp,"uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat());
    else
        Shader::Uniform(sp,"uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat()*vrController::ModelMat_);

    glm::vec3 dir = glm::vec3(vrController::RotateMat_ * glm::vec4(.0,.0,-1.0,1.0));
    if(dir.z < 0) glFrontFace(GL_CCW);
    else  glFrontFace(GL_CW);
    glBindVertexArray(vao_slice); glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, dimensions);

    shader_->UnUse();

    glDisable(GL_BLEND);
    if(!vrController::param_bool[dvr::CHECK_MASKON]){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
}
void texvrRenderer::Draw(){
    if(!b_init_successful) init_vertices();
    if(DRAW_BAKED) {two_pass_draw(); return;}
    draw_scene();
}
void texvrRenderer::onCuttingChange(float percent){
    int cut_id = int(dimensions * percent);
    GLuint sp = shader_->Use();
        Shader::Uniform(sp, "u_cut_texz", 1.0f-dimension_inv * cut_id);
    shader_->UnUse();
}
void texvrRenderer::updatePrecomputation(GLuint sp) {
    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_tex[dvr::TUNE_OVERALL]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_tex[dvr::TUNE_LOWEST]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_tex[dvr::TUNE_CUTOFF]);
}

void texvrRenderer::two_pass_draw() {
    if(!baked_dirty_) {screenQuad::instance()->Draw(); return;}
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    //render to texture
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    screenQuad::instance()->Draw();
    baked_dirty_ = false;
}

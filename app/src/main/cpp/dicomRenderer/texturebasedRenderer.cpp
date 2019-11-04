#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
texvrRenderer::texvrRenderer(bool screen_baked)
:DRAW_BAKED(screen_baked){
    Mesh::InitQuadWithTex(vao_screen_, quad_vertices_tex_standard, 4, quad_indices, 6);

    dimensions = int(vrController::tex_volume->Depth() * DENSE_FACTOR);
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

    //program
    shader_ = new Shader();
    if(!shader_->AddShaderFile(GL_VERTEX_SHADER,"shaders/textureVolume.vert")
       ||!shader_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/textureVolume.frag")
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create texture based shader program===");

    shader_baked_ = new Shader();
    if(!shader_baked_->AddShaderFile(GL_VERTEX_SHADER,"shaders/quad.vert")
       ||!shader_baked_->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/quad.frag")
       ||!shader_baked_->CompileAndLink())
        LOGE("TextureBas===Failed to create tex-baked shader program===");
    onCuttingChange(.0f);
}
void texvrRenderer::draw_scene(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!vrController::param_bool_map["maskon"]){
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
    if(!vrController::param_bool_map["maskon"]){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
}
void texvrRenderer::Draw(){
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
    Shader::Uniform(sp,"uOpacitys.overall", vrController::param_value_map["overall"]);
    Shader::Uniform(sp,"uOpacitys.lowbound", vrController::param_value_map["lowbound"]);
    Shader::Uniform(sp,"uOpacitys.cutoff", vrController::param_value_map["cutoff"]);
}
void texvrRenderer::draw_screen_quad(){
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    //render to screen
    GLuint sp = shader_baked_->Use();
    glActiveTexture(GL_TEXTURE0+BAKED_TEX_SCREEN_ID);
    glBindTexture(GL_TEXTURE_2D, baked_screen->GLTexture());
    Shader::Uniform(sp, "uSampler", BAKED_TEX_SCREEN_ID);
    glBindVertexArray(vao_screen_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_baked_->UnUse();
}
void texvrRenderer::two_pass_draw() {
    GLenum err;

    if(!baked_dirty_) {draw_screen_quad(); return;}
    if(!frame_buff_){
        float width = vrController::_screen_w, height = vrController::_screen_h;
//        int vsize= width* height;
//        GLbyte * vdata = new GLbyte[vsize * 4];
//        memset(vdata, 0xff, vsize * 4 * sizeof(GLbyte));
        baked_screen = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, nullptr);

        Texture::initFBO(frame_buff_,
                         baked_screen, new Texture(GL_R8, GL_RED, GL_UNSIGNED_BYTE, width, height, nullptr));
        if((err = glGetError()) != GL_NO_ERROR){
            LOGE("=====err2 %d, ",err);
        }
        BAKED_TEX_SCREEN_ID = vrController::BAKED_RAY_ID + 1;
    }

    //render to texture
    glViewport(0,0,vrController::_screen_w, vrController::_screen_h);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    draw_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    draw_screen_quad();
    baked_dirty_ = false;

    if((err = glGetError()) != GL_NO_ERROR){
        LOGE("=====err3 %d, ",err);
    }
}

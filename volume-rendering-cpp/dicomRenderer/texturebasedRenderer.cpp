#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
#include "screenQuad.h"

texvrRenderer::texvrRenderer(bool screen_baked)
        :DRAW_BAKED(screen_baked){
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create texture based shader program===");
    Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT] = "";Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG]="";
    setCuttingPlane(.0f);
}

void texvrRenderer::init_vertices(GLuint &vao_slice, GLuint& vbo_instance,bool is_front){
    glGenBuffers(1, &vbo_instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_DIMENSIONS, nullptr, GL_STATIC_DRAW);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.
    b_init_successful = true;

    update_instance_data(vbo_instance, is_front);
}
void texvrRenderer::update_instance_data(GLuint& vbo_instance, bool is_front){
    if(dimensions == 0) return;
    glm::vec2 *zInfos = new glm::vec2[dimensions];

    float zTex = .0f;
    float step = 1.0f / dimensions;
    if(is_front){
        float mappedZVal = - (dimensions - 1) / 2.0f * step; //-scale_inv;// + 0.5f* (MAX_DIMENSIONS - dimensions)/MAX_DIMENSIONS;
        for (int i = 0; i < dimensions; i++){
            zInfos[i].x = mappedZVal*vol_thickness_factor; zInfos[i].y = zTex;
            mappedZVal+=step; zTex+=dimension_inv;
        }
    }else{
        float mappedZVal = (dimensions - 1) / 2.0f * step; //-scale_inv;// + 0.5f* (MAX_DIMENSIONS - dimensions)/MAX_DIMENSIONS;
        for (int i = 0; i < dimensions; i++){
            zInfos[i].x = mappedZVal*vol_thickness_factor; zInfos[i].y = zTex;
            mappedZVal-=step; zTex+=dimension_inv;
        }
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
    //     glm::mat4 modelmat =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(.0,.0,1.0f)) 
    //   * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    Shader::Uniform(sp, "uMVP", Manager::camera->getProjMat() * Manager::camera->getViewMat() * modelmat);
    Shader::Uniform(sp, "u_cut", Manager::param_bool[dvr::CHECK_CUTTING]);

    //for backface rendering! don't erase
    glm::mat4 rotmat = vrController::instance()->getRotationMatrix();
    if(rotmat[2][2] > 0){
        Shader::Uniform(sp, "u_cut_texz", 1.0f-dimension_inv * cut_id);
        Shader::Uniform(sp, "u_front", true);
        glFrontFace(GL_CCW);
        glBindVertexArray(vao_front); glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, dimensions);
    }else{
        Shader::Uniform(sp, "u_cut_texz", dimension_inv * cut_id);
        Shader::Uniform(sp, "u_front", false);
        glFrontFace(GL_CW);
        glBindVertexArray(vao_back); glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, dimensions);
    }
    glFrontFace(GL_CCW);
    glBindVertexArray(0);

    shader_->UnUse();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void texvrRenderer::Draw(){
    if(!b_init_successful) {init_vertices(vao_front,vbo_front,true);init_vertices(vao_back,vbo_back,false);}
    if(DRAW_BAKED) {draw_baked(); return;}
    draw_scene();
}

void texvrRenderer::draw_baked() {
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

void texvrRenderer::setDimension(int dims, float thickness){
    dimensions = int(dims * DENSE_FACTOR);dimension_inv = 1.0f / dimensions;
    if(thickness > 0){
        vol_thickness_factor = thickness;
    }else{
        if(dims > 200) vol_thickness_factor = 0.5;
        else if(dims > 100) vol_thickness_factor = dims / 300.f;
        else vol_thickness_factor = dims / 200.f;
    }
    update_instance_data(vbo_front, true);
    update_instance_data(vbo_back, false);

}
void texvrRenderer::setCuttingPlane(float percent){
    cut_id = int(dimensions * percent);
    baked_dirty_ = true;
}
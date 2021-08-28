#include <vrController.h>
#include <GLPipeline/Primitive.h>
#include "texturebasedRenderer.h"
#include "screenQuad.h"

texvrRenderer::texvrRenderer()
:baseDicomRenderer(){
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("TextureBas===Failed to create texture based shader program===");
    init_vertices(vao_front,vbo_front);
    init_vertices(vao_back,vbo_back);

//    Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_VERT] = "";Manager::shader_contents[dvr::SHADER_TEXTUREVOLUME_FRAG]="";
    setCuttingPlane(.0f);
    float tmp[] ={Manager::indiv_rendering_params[0]};
    setRenderingParameters(tmp);
}
void texvrRenderer::init_vertices(GLuint &vao, GLuint& vbo){
    GLuint ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * MAX_DIMENSIONS, nullptr, GL_DYNAMIC_DRAW);

    if(m_indices == nullptr){
        m_indices = new unsigned int[6*MAX_DIMENSIONS];
        for(int i=0, idk=0;i<MAX_DIMENSIONS;i++, idk+=6){
            for(int k=0;k<6;k++)m_indices[idk+k]=quad_indices[k]+4*i;
        }
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6*MAX_DIMENSIONS, m_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void texvrRenderer::update_instance_data(){
    if(dimension_draw == 0) return;
    if(!b_init_successful){
        m_vertices_front = new float[12 * MAX_DIMENSIONS]; m_vertices_back= new float[12*MAX_DIMENSIONS];
        for(int i=0, idj=0;i<MAX_DIMENSIONS;i++,idj+=12){
            for(int j=0; j<12; j++){
                m_vertices_front[idj+j]= quad_vertices_3d[j];m_vertices_back[idj+j] = quad_vertices_3d[j];
            }
        }
        b_init_successful = true;
    }

    float mappedZVal = -0.5f;
    for(int i=0, id=0;i<dimension_draw;i++,id+=12){
        m_vertices_front[id+2]=mappedZVal;m_vertices_front[id+5]=mappedZVal;m_vertices_front[id+8]=mappedZVal;m_vertices_front[id+11]=mappedZVal;
        mappedZVal+= dimension_draw_inv;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_front);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*dimension_draw *12, m_vertices_front);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mappedZVal = 0.5f;
    for(int i=0, id=0;i<dimension_draw;i++,id+=12){
        m_vertices_back[id+2]=m_vertices_back[id+5]=m_vertices_back[id+8]=m_vertices_back[id+11]=mappedZVal;
        mappedZVal-=dimension_draw_inv;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_back);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*dimension_draw *12, m_vertices_back);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void texvrRenderer::draw_scene(glm::mat4 model_mat){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
    Shader::Uniform(sp, "uSampler_baked", dvr::BAKED_TEX_ID);

    Shader::Uniform(sp, "uMVP", Manager::camera->getVPMat() * model_mat);
    Shader::Uniform(sp, "uVolumeThickness", vol_thickness_factor);
    // Shader::Uniform(sp, "u_cut", Manager::param_bool[dvr::CHECK_CUTTING]);

    if(Manager::IsCuttingEnabled())shader_->EnableKeyword("CUTTING_PLANE");
    else shader_->DisableKeyword("CUTTING_PLANE");

    vrController::instance()->setCuttingParams(sp);

    //for backface rendering! don't erase
    glm::vec3 dir = Manager::camera->getViewDirection();
    bool is_front = (model_mat[2][2] * dir.z) < .0f;
    Shader::Uniform(sp, "u_cut_texz", is_front?1.0f-dimension_draw_inv * cut_id : dimension_draw_inv * cut_id);
    if(is_front){
        glFrontFace(GL_CCW); glBindVertexArray(vao_front);
    }else{
        glFrontFace(GL_CW); glBindVertexArray(vao_back);
    }
    glDrawElements(GL_TRIANGLES, 6*dimension_draw, GL_UNSIGNED_INT, m_indices);
    glBindVertexArray(0);

    shader_->UnUse();
    glFrontFace(GL_CCW);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void texvrRenderer::Draw(bool pre_draw, glm::mat4 model_mat){
    if(m_instance_data_dirty){
        update_instance_data();
        m_instance_data_dirty = false;
    }
    if(pre_draw || Manager::param_bool[dvr::CHECK_AR_ENABLED]) draw_baked(model_mat);
    else draw_scene(model_mat);
}

void texvrRenderer::draw_baked(glm::mat4 model_mat) {
    if(!Manager::param_bool[dvr::CHECK_AR_ENABLED] && !baked_dirty_) return;
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    //render to texture
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(model_mat);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}

void texvrRenderer::setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale){
    baseDicomRenderer::setDimension(vol_dim, vol_scale);
    vol_thickness_factor = vol_scale.z;
    on_update_dimension_draw();
}
void texvrRenderer::on_update_dimension_draw(){
    dimension_draw = fmin(float(dimensions_origin) * dense_factor, MAX_DIMENSIONS);
    dimension_draw_inv = 1.0f / float(dimension_draw);
    m_instance_data_dirty = true;
}

void texvrRenderer::setRenderingParameters(float* values){
    if(values[0] == dense_factor) return;
    dense_factor = values[0];
    on_update_dimension_draw();
}

void texvrRenderer::setCuttingPlane(float percent){
    cut_id = int(dimension_draw * percent);
    baked_dirty_ = true;
}
void texvrRenderer::setCuttingPlaneDelta(int delta){
    cut_id = ((int)fmax(0, cut_id + delta))%dimension_draw;
    baked_dirty_ = true;
}

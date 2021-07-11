#include <vrController.h>
#include "organMeshRenderer.h"
#include "screenQuad.h"
#include <GLPipeline/Primitive.h>
organMeshRenderer::organMeshRenderer(){
    shader_draw_ = new Shader();
    if(!shader_draw_->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_MC_VERT])
            ||!shader_draw_->AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_MC_FRAG])
            ||!shader_draw_->CompileAndLink())
        LOGE("OrganMesh===Failed to create marching cube drawing shader===");
    
    shader_ = new Shader();
    if(!shader_->AddShader(GL_COMPUTE_SHADER,Manager::shader_contents[dvr::SHADER_MARCHING_CUBE_GLSL])
        ||!shader_->CompileAndLink())
        LOGE("OrganMesh===Failed to create mesh shader program===");
    baked_dirty_=true;
}
organMeshRenderer::~organMeshRenderer(){
    glDeleteBuffers(1, &buffer_triangle_table);
    glDeleteBuffers(1, &buffer_configuration_table);
    glDeleteBuffers(1, &buffer_vertices);
    glDeleteVertexArrays(1, &vao_);
}

void organMeshRenderer::Setup(int h, int w, int d){
    volume_size = glm::vec3(int(h*grid_factor), int(w*grid_factor), int(d*grid_factor));
    max_number_of_vertices = volume_size.x * volume_size.y * volume_size.z * max_triangles_per_cell * max_vertices_per_triangle;

    const size_t buffer_size = 2*max_number_of_vertices * sizeof(glm::vec4);
    LOGI("Max number of vertices: %d", max_number_of_vertices);
    LOGI("Buffer size (in bytes): ", buffer_size);
    if(vao_ == 0){
        glGenBuffers(1, &buffer_triangle_table);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_triangle_table);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256 * 16, triangle_table, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &buffer_configuration_table);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_configuration_table);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256, edge_table, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }else{
        glDeleteBuffers(1, &buffer_vertices);
	    glDeleteVertexArrays(1, &vao_);
    }
    {
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &buffer_vertices);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_vertices);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    baked_dirty_=true;
}
void organMeshRenderer::draw_scene(glm::mat4 model_mat){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint dsp = shader_draw_->Use();
    Shader::Uniform(dsp, "uMVP", Manager::camera->getVPMat() * model_mat);
    Shader::Uniform(dsp, "uDrawWire", Manager::param_bool[dvr::CHECK_POLYGON_WIREFRAME]);
    
    bool is_cut_enable = Manager::IsCuttingEnabled();
    Shader::Uniform(dsp, "uEnableCut", is_cut_enable);

    if(is_cut_enable){
        glm::vec3 pp,pn;
        vrController::instance()->getCuttingPlane(pp, pn);
        float d = pp.x*pn.x+pp.y*pn.y+pp.z*pn.z;
        Shader::Uniform(dsp, "uPlaneVec", glm::vec4(pn.x, pn.y, pn.z, -d));
    }

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, max_number_of_vertices);
    glBindVertexArray(0);

    shader_draw_->UnUse();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}
void organMeshRenderer::draw_baked(glm::mat4 model_mat){
    if(!Manager::param_bool[dvr::CHECK_AR_ENABLED] && !baked_dirty_) return;
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(model_mat);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}
void organMeshRenderer::Draw(bool pre_draw, GLuint mask_tex, glm::mat4 model_mat) {
    if(Manager::baked_dirty_){
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_vertices);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer_triangle_table);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer_configuration_table);
        glBindImageTexture(2, mask_tex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);

        GLuint sp = shader_->Use();
        Shader::Uniform(sp, "u_maskbits", Manager::getMaskBits());
        Shader::Uniform(sp, "u_organ_num", Manager::getMaskNum());
        Shader::Uniform(sp, "u_gridsize", volume_size);

        glDispatchCompute((GLuint)(volume_size.x + 7) / 8, (GLuint)(volume_size.y + 7) / 8, (GLuint)(volume_size.z + 7) / 8);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glBindImageTexture(2, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
        shader_->UnUse();
    }
    if(pre_draw || Manager::param_bool[dvr::CHECK_AR_ENABLED]) draw_baked(model_mat);
    else draw_scene(model_mat);
}
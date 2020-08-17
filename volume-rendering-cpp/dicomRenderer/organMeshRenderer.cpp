#include <vrController.h>
#include "organMeshRenderer.h"
#include "screenQuad.h"
#include <GLPipeline/Primitive.h>
organMeshRenderer::organMeshRenderer(){
    // //geometry
    // Mesh::InitQuadWithTex(vao_cube_, cuboid_with_texture, 8, cuboid_indices, 36);
    Mesh::InitQuadWithTex(vao_test, cuboid_with_texture, 8, cuboid_indices, 36);
    shader_draw_ = new Shader();
    if(!shader_draw_->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_MC_VERT])
            ||!shader_draw_->AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_MC_FRAG])
            ||!shader_draw_->CompileAndLink())
        LOGE("OrganMesh===Failed to create marching cube drawing shader===");
}
void organMeshRenderer::Setup(int h, int w, int d, int mask_id){
    volume_size = glm::vec3(h,w,d);
    mask_id_ = mask_id;
    shader_ = new Shader();
    if(!shader_->AddShader(GL_COMPUTE_SHADER,Manager::shader_contents[dvr::SHADER_MARCHING_CUBE_GLSL])
            ||!shader_->CompileAndLink())
        LOGE("OrganMesh===Failed to create mesh shader program===");

     max_number_of_vertices = static_cast<size_t>(volume_size.x) * 
                                          static_cast<size_t>(volume_size.y) * 
                                          static_cast<size_t>(volume_size.z) * max_triangles_per_cell * max_vertices_per_triangle;

    const size_t buffer_size = 2*max_number_of_vertices * sizeof(glm::vec4);
    LOGI("Max number of vertices: %d", max_number_of_vertices);
    LOGI("Buffer size (in bytes): ", buffer_size);

    {
        glGenBuffers(1, &buffer_triangle_table);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_triangle_table);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256 * 16, triangle_table, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &buffer_configuration_table);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_configuration_table);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 256, edge_table, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    }
    {
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        glGenBuffers(1, &buffer_vertices);

        glBindBuffer(GL_ARRAY_BUFFER, buffer_vertices);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    //init shader clear
//    shader_clear= new Shader();
//    if(!shader_clear->AddShader(GL_COMPUTE_SHADER,Manager::shader_contents[dvr::SHADER_MARCHING_CUBE_CLEAR_GLSL])
//            ||!shader_clear->CompileAndLink())
//        LOGE("OrganMesh Clear===Failed to create mesh shader program===");
}
void organMeshRenderer::Draw() {
    if(!initialized){
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_vertices);
//        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer_normals);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer_triangle_table);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer_configuration_table);
//        if(mask_id_>=dvr::ORGAN_END) glBindImageTexture(0, vrController::instance()->getMaskTex(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8UI);
//        else glBindImageTexture(0, vrController::instance()->getMaskTex(mask_id_), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8UI);
        glBindImageTexture(0, vrController::instance()->getMaskTex(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

        //do clear
        // a debug sphere
        //     GLuint spc = shader_clear->Use();
        //     glDispatchCompute((GLuint)(volume_size.x + 7) / 8, (GLuint)(volume_size.y + 7) / 8, (GLuint)(volume_size.z + 7) / 8);
        //     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        // shader_clear->UnUse();
        initialized = true;
    }
    if(Manager::baked_dirty_){
        GLuint sp = shader_->Use();
        Shader::Uniform(sp, "u_multiple", mask_id_>=dvr::ORGAN_END);
        Shader::Uniform(sp, "u_maskbits", vrController::instance()->mask_bits_);
        Shader::Uniform(sp, "u_organ_num", vrController::instance()->mask_num_-1);
        Shader::Uniform(sp, "u_mask_id", (unsigned int)pow(2.0f, (int)mask_id_));
        glDispatchCompute((GLuint)(volume_size.x + 7) / 8, (GLuint)(volume_size.y + 7) / 8, (GLuint)(volume_size.z + 7) / 8);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        shader_->UnUse();
        Manager::baked_dirty_ = false;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

//    if(Manager::param_bool[dvr::CHECK_POLYGON_WIREFRAME])glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    GLuint dsp = shader_draw_->Use();
    Shader::Uniform(dsp, "uMVP", 
    Manager::camera->getProjMat() * Manager::camera->getViewMat() 
    * vrController::instance()->getModelMatrix(true)
    * glm::scale(glm::mat4(1.0), glm::vec3(0.5f))
    * tex2mesh_model);
    Shader::Uniform(dsp, "uDrawWire", Manager::param_bool[dvr::CHECK_POLYGON_WIREFRAME]);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, max_number_of_vertices);
    glBindVertexArray(0);

    shader_draw_->UnUse();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void organMeshRenderer::SetOffsetScale(int ori_h, int ori_w, int ori_d, int nh, int nw, int nd, int offy, int offx, int offz){
    float dfy = 128/nh;
    glm::vec3 ss = glm::vec3((float)nw/ori_w,(float)nh/ori_h,(float)nd/ori_d);
    glm::mat4 sscale = glm::scale(glm::mat4(1.0),ss);

    int coffx = offx+0.5*nw;
    float fx = (coffx-0.5*ori_w)/ori_w;
    int fx_s = (fx>0)?1:-1;

    offy = 512-(offy+nh);
    int coffy = offy+0.5*nh;
    float fy = (coffy - 0.5*ori_h) / ori_h;
    int fy_s = (fy>0)?1:-1;

    int coffz = offz+0.5*nd;
    float fz = (coffz - 0.5*ori_d) / ori_d;
    int fz_s = (fz>0)?1:-1;

//    std::cout<<"offset "<<offx<<" "<<fx<<std::endl;
    glm::mat4 offset_matb=glm::mat4(1.0);
    offset_matb = glm::translate(glm::mat4(1.0), glm::vec3(-fx, -fy_s*fy, -fz));

    glm::mat4 offset_mat=glm::mat4(1.0);
    offset_mat = glm::translate(glm::mat4(1.0), glm::vec3(
        fx_s*((float)offx/ori_w+fx *ss.x*1.0f),
        fy_s*((float)offy/ori_h+fy *ss.y*0.5f),
        fz_s*((float)offz/ori_d+fz *ss.z*0.5f)
    ));
        
    tex2mesh_model = offset_mat*sscale*offset_matb;
}
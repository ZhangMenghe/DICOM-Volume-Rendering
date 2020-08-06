#include <vrController.h>
#include "organMeshRenderer.h"
#include "screenQuad.h"
#include <GLPipeline/Primitive.h>
#include <Utils/fileUtils.h>
organMeshRenderer::organMeshRenderer(){
    // //geometry
    // Mesh::InitQuadWithTex(vao_cube_, cuboid_with_texture, 8, cuboid_indices, 36);
    Mesh::InitQuadWithTex(vao_test, cuboid_with_texture, 8, cuboid_indices, 36);
    shader_draw_ = new Shader();
    if(!shader_draw_->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_MC_VERT])
            ||!shader_draw_->AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_MC_FRAG])
            ||!shader_draw_->CompileAndLink())
        LOGE("OrganMesh===Failed to create marching cube drawing shader===");
    init_buffer();
}
void organMeshRenderer::init_buffer(){
    shader_ = new Shader();
    if(!shader_->AddShader(GL_COMPUTE_SHADER,Manager::shader_contents[dvr::SHADER_MARCHING_CUBE_GLSL])
            ||!shader_->CompileAndLink())
        LOGE("OrganMesh===Failed to create mesh shader program===");

     max_number_of_vertices = static_cast<size_t>(volume_size.x) * 
                                          static_cast<size_t>(volume_size.y) * 
                                          static_cast<size_t>(volume_size.z) * max_triangles_per_cell * max_vertices_per_triangle;

    const size_t buffer_size = max_number_of_vertices * sizeof(glm::vec4);
    std::cout << "Max number of vertices: " << max_number_of_vertices << std::endl;
    std::cout << "Buffer size (in bytes): " << buffer_size << std::endl;

    {
        glCreateBuffers(1, &buffer_vertices);
        glNamedBufferStorage(buffer_vertices, buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &buffer_normals);
        glNamedBufferStorage(buffer_normals, buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    {

        glCreateBuffers(1, &buffer_triangle_table);
        glNamedBufferStorage(buffer_triangle_table, sizeof(int) * 256 * 16, triangle_table, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &buffer_configuration_table);
        glNamedBufferStorage(buffer_configuration_table, sizeof(int) * 256, edge_table, GL_DYNAMIC_STORAGE_BIT);
    }
        {
        glCreateVertexArrays(1, &vao_);

        glEnableVertexArrayAttrib(vao_, 0);
        glEnableVertexArrayAttrib(vao_, 1);

        glVertexArrayVertexBuffer(vao_, 0, buffer_vertices, 0, sizeof(glm::vec4));
        glVertexArrayVertexBuffer(vao_, 1, buffer_normals, 0, sizeof(glm::vec4));
      
        glVertexArrayAttribFormat(vao_, 0, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribFormat(vao_, 1, 4, GL_FLOAT, false, 0);
       
        glVertexArrayAttribBinding(vao_, 0, 0);
        glVertexArrayAttribBinding(vao_, 1, 1);
    }

    //init shader clear
    shader_clear= new Shader();
    if(!shader_clear->AddShader(GL_COMPUTE_SHADER,Manager::shader_contents[dvr::SHADER_MARCHING_CUBE_CLEAR_GLSL])
            ||!shader_clear->CompileAndLink())
        LOGE("OrganMesh Clear===Failed to create mesh shader program===");
}
void organMeshRenderer::Draw() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer_vertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer_normals);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer_triangle_table);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer_configuration_table);
    glBindImageTexture(0, vrController::instance()->getMaskTex(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8UI);

    //do clear
    // a debug sphere
//     GLuint spc = shader_clear->Use();
//     glDispatchCompute((GLuint)(volume_size.x + 7) / 8, (GLuint)(volume_size.y + 7) / 8, (GLuint)(volume_size.z + 7) / 8);
//     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
// shader_clear->UnUse();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    if(wireframe)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    

    GLuint sp = shader_->Use();
    
    glDispatchCompute((GLuint)(volume_size.x + 7) / 8, (GLuint)(volume_size.y + 7) / 8, (GLuint)(volume_size.z + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    shader_->UnUse();

    GLuint dsp = shader_draw_->Use();
    Shader::Uniform(dsp, "uMVP", 
    Manager::camera->getProjMat() * Manager::camera->getViewMat() *vrController::instance()->getModelMatrix(true)
    * glm::scale(glm::mat4(1.0), glm::vec3(0.5f)));

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, max_number_of_vertices);
    glBindVertexArray(0);

    shader_draw_->UnUse();
        glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

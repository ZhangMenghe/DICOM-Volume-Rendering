#ifndef ORGAN_MESH_RENDERER_H
#define ORGAN_MESH_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
class organMeshRenderer{
private:
    Shader* shader_= nullptr, *shader_draw_, *shader_clear;
    GLuint vao_= 0, vao_test;
    bool wireframe = true;

    uint32_t buffer_vertices;
    uint32_t buffer_normals;
    uint32_t buffer_triangle_table;
    uint32_t buffer_configuration_table;
    size_t buffer_size;
    size_t max_number_of_vertices;
    const size_t max_triangles_per_cell = 5;
    const size_t max_vertices_per_triangle = 3;
    glm::vec3 volume_size = glm::vec3(128,128,41);
  
    void init_buffer();
public:
    organMeshRenderer();
    void Draw();
};
#endif
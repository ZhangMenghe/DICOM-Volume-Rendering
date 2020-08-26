#ifndef ORGAN_MESH_RENDERER_H
#define ORGAN_MESH_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
class organMeshRenderer{
private:
    const float grid_factor = 0.25f;
    const size_t max_triangles_per_cell = 5;
    const size_t max_vertices_per_triangle = 3;

    Shader* shader_= nullptr, *shader_draw_;
    GLuint vao_= 0;

    GLuint buffer_vertices=0;
    GLuint buffer_triangle_table=0;
    GLuint buffer_configuration_table=0;

    size_t buffer_size;
    size_t max_number_of_vertices;
    glm::vec3 volume_size;
  
public:
    organMeshRenderer();
    ~organMeshRenderer();

    void Setup(int h, int w, int d);
    void Draw();
};
#endif
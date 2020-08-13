#ifndef ORGAN_MESH_RENDERER_H
#define ORGAN_MESH_RENDERER_H

#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>
class organMeshRenderer{
private:
    Shader* shader_= nullptr, *shader_draw_, *shader_clear;
    GLuint vao_= 0, vao_test;

    bool initialized = false;
    uint32_t buffer_vertices;
    uint32_t buffer_normals;
    uint32_t buffer_triangle_table;
    uint32_t buffer_configuration_table;
    size_t buffer_size;
    size_t max_number_of_vertices;
    const size_t max_triangles_per_cell = 5;
    const size_t max_vertices_per_triangle = 3;
    // glm::vec3 volume_size = glm::vec3(128,128,41);
    // glm::vec3 volume_size = glm::vec3(74,120,81);
    glm::vec3 volume_size = glm::vec3(512,512,164);
    int mask_id_;
    bool baked_dirty = true;
    glm::mat4 tex2mesh_model = glm::mat4(1.0f);
  
public:
    organMeshRenderer();
    void Setup(int h, int w, int d, int mask_id);
    void SetOffsetScale(int ori_h, int ori_w, int ori_d, int nh, int nw, int nd, int offy, int offx, int offz);
    //     sscale = glm::scale(glm::mat4(1.0), glm::vec3(volume_size.x/ori_w,volume_size.y/ori_h,volume_size.z/ori_d));
    //     offset_mat = glm::translate(glm::mat4(1.0), glm::vec3(offx/ori_w, offy/ori_h, offz/ori_d)*0.125f);
    // }
    void Draw();
    void DirtyBaked(){baked_dirty = true;}
};
#endif
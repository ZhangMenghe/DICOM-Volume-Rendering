#ifndef VIEW_ALIGNED_SLICING_RENDERER_H
#define VIEW_ALIGNED_SLICING_RENDERER_H
// This method follows Algorthm 39-2. View-Aligned Slicing for Volume
// Rendering found in Chapter 39 "Volume Rendering Techniques" by
// Milan Ikits, Joe Kniss, Aaron Lefohn, and Charles Hansen of GPU Gems.
// Simian, http://www.cs.utah.edu/~jmk/simian/

#include "baseDicomRenderer.h"

class ViewAlignedSlicingRenderer:public baseDicomRenderer{
private:
    const static int MAX_DIMENSIONS = 200;
    GLuint m_vao, m_vbo, m_ibo;
    float* m_vertices=nullptr;
    unsigned int *m_indices;
    int m_slice_num;
    float m_sampling_rate = 0.3f;

    int m_vertices_num, m_indices_num;

    //    glm::vec3 m_last_vec3=glm::vec3(1e6);
    //    float m_slice_amount = .0f, m_slice_count = 0;

    //for screen baking
    void draw_scene(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);

public:
    ViewAlignedSlicingRenderer();
//    void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale);
//    void setCuttingPlane(float percent);
//    void setCuttingPlaneDelta(int delta);
    void Draw(bool pre_draw, glm::mat4 model_mat);
    void UpdateVertices(glm::mat4 model_mat);
    void setRenderingParameters(float* values);
};
#endif
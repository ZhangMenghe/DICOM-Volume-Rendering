#ifndef VIEW_ALIGNED_SLICING_RENDERER_H
#define VIEW_ALIGNED_SLICING_RENDERER_H
// This method follows Algorthm 39-2. View-Aligned Slicing for Volume
// Rendering found in Chapter 39 "Volume Rendering Techniques" by
// Milan Ikits, Joe Kniss, Aaron Lefohn, and Charles Hansen of GPU Gems.
// Simian, http://www.cs.utah.edu/~jmk/simian/

#include "baseDicomRenderer.h"

class ViewAlignedSlicingRenderer:public baseDicomRenderer{
private:
//    float vol_thickness_factor = 1.0f;
//    const float SLICE_SAMPLE_RATE = 0.4f;
    const static int MAX_DIMENSIONS = 200;
    GLuint m_vaos[MAX_DIMENSIONS], m_vbos[MAX_DIMENSIONS];//, m_ebos[MAX_DIMENSIONS];
    int m_indice_num[MAX_DIMENSIONS];
    int m_slice_num;
    float m_sampling_rate = 0.3f;
//    float m_slice_amount = .0f, m_slice_count = 0;
    const unsigned int m_indices_data[12] = {
            0,1,2,0,2,3,0,3,4,0,4,5
    };
    glm::vec3 m_last_vec3=glm::vec3(1e6);
    bool m_right_order;
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
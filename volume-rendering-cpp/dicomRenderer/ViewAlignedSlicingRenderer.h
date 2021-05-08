#ifndef VIEW_ALIGNED_SLICING_RENDERER_H
#define VIEW_ALIGNED_SLICING_RENDERER_H
// This method follows Algorthm 39-2. View-Aligned Slicing for Volume
// Rendering found in Chapter 39 "Volume Rendering Techniques" by
// Milan Ikits, Joe Kniss, Aaron Lefohn, and Charles Hansen of GPU Gems.
// Simian, http://www.cs.utah.edu/~jmk/simian/

#include "baseDicomRenderer.h"

class ViewAlignedSlicingRenderer:public baseDicomRenderer{
private:
    float vol_thickness_factor = 1.0f;
    const float SLICE_SAMPLE_RATE = 0.2f;
    const static int MAX_DIMENSIONS = 200;
    GLuint m_vaos[MAX_DIMENSIONS], m_vbos[MAX_DIMENSIONS];//, m_ebos[MAX_DIMENSIONS];
    int m_indice_num[MAX_DIMENSIONS];
    int m_slice_num;
    const unsigned int m_indices_data[12] = {
            0,1,2,0,2,3,0,3,4,0,4,5
    };

    //for screen baking
    void draw_scene(glm::mat4 model_mat);
    void draw_baked(glm::mat4 model_mat);
    void update_instance_data(glm::mat4 model_mat);

public:
    ViewAlignedSlicingRenderer();
    void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale);
    void setCuttingPlane(float percent);
    void setCuttingPlaneDelta(int delta);
    void Draw(bool pre_draw, glm::mat4 model_mat);
};
#endif
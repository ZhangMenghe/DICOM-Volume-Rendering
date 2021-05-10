#ifndef BASE_DICOM_RENDERER_H
#define BASE_DICOM_RENDERER_H

#include <GLPipeline/Shader.h>
class baseDicomRenderer{
protected:
    const float DENSE_FACTOR = 1.0f;

    Shader* shader_;

    GLuint frame_buff_;
    bool baked_dirty_;

    int dimensions; float dimension_inv;

    virtual void draw_scene(glm::mat4 model_mat) = 0;
    virtual void draw_baked(glm::mat4 model_mat) = 0;

public:
    baseDicomRenderer(){
        baked_dirty_ = true;frame_buff_=0;
    }
    virtual void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale){
        dimensions = int(vol_dim.z * DENSE_FACTOR);dimension_inv = 1.0f / dimensions;
    }
    virtual void setCuttingPlane(float percent){}
    virtual void setCuttingPlaneDelta(int delta){}
    virtual void UpdateVertices(glm::mat4 model_mat){}
    virtual void Draw(bool pre_draw, glm::mat4 model_mat) = 0;
    virtual void dirtyPrecompute(){baked_dirty_ = true;}
    virtual bool isPrecomputeDirty(){return baked_dirty_;}
};
#endif
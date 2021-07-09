#ifndef BASE_DICOM_RENDERER_H
#define BASE_DICOM_RENDERER_H

#include <GLPipeline/Shader.h>
class baseDicomRenderer{
protected:
    Shader* shader_;

    GLuint frame_buff_;
    bool baked_dirty_;
    bool vertices_dirty_ = false;

    int dimensions_origin;

    virtual void draw_scene(glm::mat4 model_mat) = 0;
    virtual void draw_baked(glm::mat4 model_mat) = 0;

public:
    baseDicomRenderer(){
        baked_dirty_ = true;frame_buff_=0;
    }
    virtual void setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale){dimensions_origin = vol_dim.z;}
    virtual void setCuttingPlane(float percent){}
    virtual void setCuttingPlaneDelta(int delta){}
    virtual void UpdateVertices(glm::mat4 model_mat){vertices_dirty_=false;}
    virtual void Draw(bool pre_draw, glm::mat4 model_mat) = 0;
    virtual void dirtyPrecompute(){baked_dirty_ = true;}
    virtual bool isPrecomputeDirty(){return baked_dirty_;}
    virtual bool isVerticesDirty(){return vertices_dirty_;}
    virtual void setRenderingParameters(float* values) = 0;
};
#endif
#ifndef DICOM_VOLUME_RENDERING_CUTTINGCONTROLLER_H
#define DICOM_VOLUME_RENDERING_CUTTINGCONTROLLER_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <GLPipeline/Shader.h>

typedef struct{
    glm::vec3 p;
    glm::vec3 n;
}cPlane;
typedef struct{
    glm::vec3 c;
    float r;
}cSphere;
typedef enum{
    PLANE = 1,
    SPHERE,
    VOLUME
}mTarget;

class cuttingController {
private:
    cPlane cplane_;
    cSphere csphere_;

    Shader* pshader = nullptr;
    GLuint pVAO_ = 0;

    glm::vec3 p_start_, p_norm_, p_point_;
    const glm::vec4 P_Points[3] = {
        glm::vec4(-1.0f,1.0f,.0f,1.0f),
                glm::vec4(1.0f,1.0f,.0f,1.0f),
                glm::vec4(-1.0f,-1.0f,.0f,1.0f)
    };
    glm::vec3 p_point_world;
    glm::vec3 p_scale = glm::vec3(1.0f);

    glm::mat4 p_rotate_mat_ = glm::mat4(1.0f);

    glm::mat4 p_p2w_mat, p_p2o_mat;
    bool p_p2o_dirty = true;
    float cmove_value = .0f;
    glm::vec4 plane_color_ = glm::vec4(0.8, 0.8,.0, 0.3);
    const float CUTTING_FACTOR = 0.00002f;
    mTarget current_target = VOLUME;

    void draw_plane();
    bool keep_cutting_position();
    void update_modelMat_o();
    void update_plane_(glm::mat4 rotMat);
    void update_plane_(glm::vec3 pNorm);

public:
    static cuttingController* _mptr;
    static cuttingController* instance();
    cuttingController();
    cuttingController(glm::vec3 ps, glm::vec3 pn);
    void setTarget(mTarget target){current_target = target;}
    void Update();
    void UpdateAndDraw();
    void setCuttingParams(GLuint sp, bool includePoints = false);

    void setCutPlane(float value);
    void setCutPlane(glm::vec3 normal);
    void setCutPlane(glm::vec3 startPoint, glm::vec3 normal);

    void onRotate(float offx, float offy);
    void onScale(float sx, float sy=-1.0f, float sz=-1.0f);
    void onTranslate(float offx, float offy);
};


#endif

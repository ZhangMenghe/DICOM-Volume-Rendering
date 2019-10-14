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
    SPHERE
}mTarget;

class cuttingController {
private:
    cPlane cplane_;
    cSphere csphere_;

    Shader* pshader = nullptr;
    GLuint pVAO_ = 0;
    mTarget mtarget = PLANE;
    glm::vec3 p_start_, p_norm_, p_point_;

    glm::mat4 p_rotate_mat_;
    void DrawPlane();
    void DrawSphere();

public:

    static cuttingController* _mptr;
    static cuttingController* instance();
    cuttingController();
    cuttingController(glm::vec3 start_p, glm::vec3 normal);

    void Draw();
    void setCuttingParams(Shader* shader);

    void setCutPlane(float percent);
    void setCutPlane(glm::vec3 normal);
    void setCutPlane(glm::vec3 startPoint, glm::vec3 normal);

    void onRotate(mTarget target, float offx, float offy);
    void onScale(mTarget target, float sx, float sy=-1.0f, float sz=-1.0f);
    void onTranslate(mTarget target, float offx, float offy);
};


#endif

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

class cuttingController {
private:
    cPlane cplane_;
    cSphere csphere_;

    Shader* pshader = nullptr;
    GLuint pVAO_ = 0;

    glm::vec3 p_start_, p_norm_, p_point_;
    void DrawPlane();
    void DrawSphere();

public:
    cuttingController();
    cuttingController(glm::vec3 start_p, glm::vec3 normal);

    void Draw();
    void setCuttingParams(Shader* shader);

    void setCutPlane(float percent);
    void setCutPlane(glm::vec3 normal);
    void setCutPlane(glm::vec3 startPoint, glm::vec3 normal);
};


#endif

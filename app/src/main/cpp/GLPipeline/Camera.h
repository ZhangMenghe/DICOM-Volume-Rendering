#ifndef DICOM_VOLUME_RENDERING_CAMERA_H
#define DICOM_VOLUME_RENDERING_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera{
    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos, _up,  _center;

    const float NEAR_PLANE = 2.f;//as close as possible
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;
    const glm::vec3 ORI_CAM_POS = glm::vec3(0.0f, .0f, 3.0f);
    const glm::vec3 ORI_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 ORI_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
    }
public:
    Camera(){
        _up = ORI_UP; _eyePos = ORI_CAM_POS;
        _center = ORI_CAM_POS + ORI_FRONT;
        updateCameraVector();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        _projMat = glm::perspective(FOV, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getCameraPosition(glm::mat4 modelMat){
        glm::mat4 inv_model = glm::inverse(modelMat);
        glm::vec4 eye_model = inv_model * glm::vec4(_eyePos.x, _eyePos.y, _eyePos.z, 1.0f);
        float inv_w = 1.0f / eye_model.w;
        return glm::vec3(eye_model.x * inv_w, eye_model.y * inv_w, eye_model.z * inv_w);
    }
    glm::vec3 getViewCenter(){return _center;}
    glm::vec3 getViewDirection(){return glm::normalize(_center - _eyePos);}

    void rotateCamera(int axis, glm::vec4 center, float offset){
        glm::vec3 rotateAxis = (axis==3)?glm::vec3(0,1,0):glm::vec3(1,0,0);
        glm::mat4 modelMat = glm::mat4(1.0);

        modelMat = glm::translate(modelMat, glm::vec3(-center.x, -center.y, -center.z));
        modelMat = glm::rotate(modelMat, offset, rotateAxis);
        modelMat = glm::translate(modelMat, glm::vec3(center.x, center.y, center.z));
        _eyePos = glm::vec3(modelMat * glm::vec4(_eyePos, 1.0));
        _center = glm::vec3(center);
        updateCameraVector();
    }
};

#endif

#ifndef DICOM_VOLUME_RENDERING_CAMERA_H
#define DICOM_VOLUME_RENDERING_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera{
    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos, _center, _up, _front, _right;

    const float NEAR_PLANE = 1.8f;//as close as possible
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;
    const glm::vec3 ORI_CAM_POS = glm::vec3(0.0f, .0f, 2.8f);
    const glm::vec3 ORI_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 ORI_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
    }
public:
    Camera(){
        _up = ORI_UP; _eyePos = ORI_CAM_POS;
        _center = ORI_CAM_POS + ORI_FRONT;
        _front = ORI_FRONT;

        updateCameraVector();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        _projMat = glm::perspective(FOV, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getViewCenter(){return _center;}
    glm::vec3 getViewDirection(){return _front;}

    void rotateCamera(int axis, glm::vec4 center, float offset){
        glm::vec3 rotateAxis = (axis==3)?glm::vec3(0,1,0):glm::vec3(1,0,0);
        glm::mat4 modelMat = glm::mat4(1.0);

        modelMat = glm::translate(modelMat, glm::vec3(-center.x, -center.y, -center.z));
        modelMat = glm::rotate(modelMat, offset, rotateAxis);
        modelMat = glm::translate(modelMat, glm::vec3(center.x, center.y, center.z));
        _eyePos = glm::vec3(modelMat * glm::vec4(_eyePos, 1.0));
        _center = glm::vec3(center);
        _front = _center - _eyePos;
//        _front = glm::vec3(modelMat * glm::vec4(_front,1.0));
//        // Also re-calculate the Right and Up vector
//        _right = glm::vec3(glm::cross(_front, ORI_UP));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//        _up = glm::normalize(glm::cross(_right, _front));
//        _center = _eyePos + _front;

        updateCameraVector();
    }
};

#endif

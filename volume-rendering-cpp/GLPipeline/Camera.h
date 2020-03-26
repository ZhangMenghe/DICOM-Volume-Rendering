#ifndef GLPIPELINE_CAMERA_H
#define GLPIPELINE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera{
    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos, _center, _up, _front, _right;
    glm::mat4 pose_;
    const char* name_;
    float fov;

    const float NEAR_PLANE = 1.8f;//as close as possible
    const float FAR_PLANE = 1000.0f;
    const float FOV_DEG = 45.0f;
    const glm::vec3 ORI_CAM_POS = glm::vec3(0.0f, .0f, 2.8f);
    const glm::vec3 ORI_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 ORI_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCameraVector(glm::mat4 modelmat){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
        pose_ = modelmat;
    }
    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
        pose_ = glm::translate(glm::mat4(1.0), _eyePos);
    }
public:
    Camera(const char* cam_name)
    :name_(cam_name){
        Reset();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        fov = FOV_DEG * (3.14f / 180.0f);
        _projMat = glm::perspective(fov, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    void setViewMat(glm::mat4 viewmat){_viewMat = viewmat;}
    void setProjMat(glm::mat4 projmat){
        fov =  2.0*atan( 1.0f/projmat[1][1] );
        _projMat = projmat;
    }

    void updateCameraPose(glm::mat4 pose){
        //pose is in column major
        _eyePos = glm::vec3(pose[3][0], pose[3][1], pose[3][2]);
        _front = glm::vec3(pose[2][0], pose[2][1], pose[2][2]);
        pose_ = pose;
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::mat4 getVPMat(){return _projMat * _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getViewDirection(){return _front;}
    glm::mat4 getCameraPose(){return pose_;}
    float getFOV(){return fov;}
    void Reset(){
        _up = ORI_UP; _eyePos = ORI_CAM_POS;
        _center = ORI_CAM_POS + ORI_FRONT;
        _front = ORI_FRONT;
        updateCameraVector();
    }

    void rotateCamera(int axis, glm::vec4 center, float offset){
        glm::vec3 rotateAxis = (axis==3)?glm::vec3(0,1,0):glm::vec3(1,0,0);
        glm::mat4 modelMat = glm::mat4(1.0);

        modelMat = glm::translate(modelMat, glm::vec3(-center.x, -center.y, -center.z));
        modelMat = glm::rotate(modelMat, offset, rotateAxis);
        modelMat = glm::translate(modelMat, glm::vec3(center.x, center.y, center.z));
        _eyePos = glm::vec3(modelMat * glm::vec4(_eyePos, 1.0));
        _center = glm::vec3(center);
        _front = _center - _eyePos;
        updateCameraVector(modelMat);
    }
};

#endif

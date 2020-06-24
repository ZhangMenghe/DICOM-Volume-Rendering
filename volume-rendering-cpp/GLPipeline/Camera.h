#ifndef GLPIPELINE_CAMERA_H
#define GLPIPELINE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
class Camera{
    const char* name_;

    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos, _center, _up, _front, _right;

    glm::mat4 pose_mat;
    float fov;

    const float NEAR_PLANE = 1.8f;//as close as possible
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;
    const glm::vec3 ORI_CAM_POS = glm::vec3(0.0f, .0f, 4.f);
    const glm::vec3 ORI_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 ORI_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
        pose_mat = glm::translate(glm::mat4(1.0), _eyePos);
    }
    void updateCameraVector(glm::mat4 model){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
        pose_mat = model;
    }
    void reset(glm::vec3 pos, glm::vec3 up, glm::vec3 center){
        _up = up; _eyePos = pos;
        _center = center;
        _front = _center - _eyePos;
        updateCameraVector();
    }
public:
    Camera(){
        reset(ORI_CAM_POS, ORI_UP, ORI_CAM_POS+ORI_FRONT);
    }
    Camera(const char* cam_name):name_(cam_name){reset(ORI_CAM_POS, ORI_UP, ORI_CAM_POS+ORI_FRONT);}
    Camera(glm::vec3 pos, glm::vec3 up, glm::vec3 center){
        reset(pos, up, center);
    }

    //setters
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        fov = FOV * (3.14f / 180.0f);
        _projMat = glm::perspective(fov, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    void setViewMat(glm::mat4 viewmat){_viewMat = viewmat;}
    void setProjMat(glm::mat4 projmat){
        fov =  2.0*atan( 1.0f/projmat[1][1] );
        _projMat = projmat;
    }

    void updateCameraPose(glm::mat4 pose) {
        //pose is in column major
        _eyePos = glm::vec3(pose[3][0], pose[3][1], pose[3][2]);
        _front = -glm::vec3(pose[2][0], pose[2][1], pose[2][2]);
        pose_mat = pose;
    }

    //getters
    float getFOV(){return fov;}
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::mat4 getVPMat(){return _projMat * _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getViewCenter(){return _center;}
    glm::vec3 getViewDirection(){return _front;}
    glm::vec3 getViewUpDirection(){return _up;}
    glm::mat4 getCameraPose(){return pose_mat;}

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
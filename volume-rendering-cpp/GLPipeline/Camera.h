#ifndef GLPIPELINE_CAMERA_H
#define GLPIPELINE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
class Camera{
    const char* name_;

    glm::mat4 _viewMat, _projMat;
    glm::mat4 poseMat_;

    glm::vec3 _eyePos, _center, _up, _front;

    float fov;

    float DEFAULT_NEAR_PLANE = 1.8f;
    float DEFAULT_FAR_PLANE = 1000.0f;
    float DEFAULT_FOV = 45.0f;
    glm::vec3 DEFAULT_CAM_POS = glm::vec3(0.0f, .0f, 4.f);
    glm::vec3 DEFAULT_CAM_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 DEFAULT_VIEW_CENTER = glm::vec3(0.0f);


    void update_cam_pose() {
        poseMat_ = glm::inverse(_viewMat);
        _eyePos = glm::vec3(poseMat_[3][0], poseMat_[3][1], poseMat_[3][2]);
        _front = -glm::vec3(poseMat_[2][0], poseMat_[2][1], poseMat_[2][2]);
    }
public:
    Camera(){
        Reset(DEFAULT_CAM_POS, DEFAULT_CAM_UP, DEFAULT_VIEW_CENTER);
    }
//    Camera(const char* cam_name)
//    :name_(cam_name){
//        reset(DEFAULT_CAM_POS, DEFAULT_CAM_UP, DEFAULT_VIEW_CENTER);
//    }
//    Camera(const char* cam_name, glm::vec3 pos, glm::vec3 up, glm::vec3 center)
//    :name_(cam_name){
//        reset(pos, up, center);
//    }
    void Reset(Camera* cam){
        Reset(cam->_eyePos, cam->_up, cam->_center);
    }
    void Reset(glm::vec3 pos, glm::vec3 up, glm::vec3 center){
        _up = up;
        _eyePos = pos;
        _center = center;
        _front = _center - _eyePos;
        _viewMat = glm::lookAt(pos, center, up);
    }

    //setters
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        fov = DEFAULT_FOV * (3.14159f / 180.0f);
        _projMat = glm::perspective(fov, screen_ratio, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
    }
    void setProjMat(glm::mat4 projmat){
        fov =  2.0*atan( 1.0f/projmat[1][1] );
        _projMat = projmat;
    }

    void setViewMat(glm::mat4 viewmat){
        _viewMat = viewmat;
        update_cam_pose();
    }

    //getters
    float getFOV(){return fov;}
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat() {return _viewMat;}
    glm::mat4 getVPMat(){return _projMat * _viewMat;}
    glm::mat4 getCameraPose(){return poseMat_;}

    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getViewDirection(){return _front;}

    glm::vec3 getViewCenter(){return _center;}
    glm::vec3 getViewUpDirection(){return _up;}

    glm::mat4 getRotationMatrixOfCameraDirection(){
        //a is the vector you want to translate to and b is where you are
        const glm::vec3 a = -_front;//src_dir;
        const glm::vec3 b = glm::vec3(0,0,1);//dest_dir;
        glm::vec3 v = glm::cross(b, a);
        float angle = acos(glm::dot(b, a) / (glm::length(b) * glm::length(a)));
        return glm::rotate(angle, v);
    }
};

#endif
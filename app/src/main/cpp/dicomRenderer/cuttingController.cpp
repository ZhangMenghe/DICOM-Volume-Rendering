#include "cuttingController.h"

#include <glm/gtx/transform.hpp>
#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include <glm/gtx/rotate_vector.hpp>
#include <AndroidUtils/mathUtils.h>
using namespace glm;

cuttingController* cuttingController::_mptr = nullptr;
cuttingController* cuttingController::instance(){
    if(!_mptr)_mptr = new cuttingController;
    return _mptr;
}
cuttingController::cuttingController(){
    mat4 vm_inv = glm::transpose(vrController::ModelMat_);
    //view dir in obj space
    p_norm_ = vec3MatNorm(vm_inv, vrController::camera->getViewDirection());

    //camera pos in obj space
//    glm::vec3 vp_obj = vec3MatNorm(vm_inv, vrController::camera->getCameraPosition());
    //cloest point
    p_start_ = glm::vec3(.0f);//cloestVertexToPlane(p_norm_, vp_obj);
    p_point_ = p_start_;

    p_rotate_mat_ = rotMatFromDir(p_norm_);

    p_p2v_mat = glm::translate(glm::mat4(1.0), p_point_)*p_rotate_mat_ * glm::scale(glm::mat4(1.0), p_scale);
    _mptr = this;
}
cuttingController::cuttingController(glm::vec3 ps, glm::vec3 pn):
p_start_(ps), p_norm_(pn){
    p_point_ = p_start_;
    p_rotate_mat_ = rotMatFromDir(pn);
    p_p2v_mat = glm::translate(glm::mat4(1.0), p_point_)*p_rotate_mat_ * glm::scale(glm::mat4(1.0), p_scale);
    _mptr = this;
}
void cuttingController::Draw(){
    update();
    if(vrController::param_bool_map["cutting"])draw_plane();
}
void cuttingController::setCuttingParams(Shader* shader){
    shader->setVec3("uSphere.center", glm::vec3(vrController::csphere_c));
    shader->setFloat("uSphere.radius", vrController::csphere_radius);

    shader->setVec3("uPlane.p", p_point_);
    shader->setVec3("uPlane.normal", p_norm_);//* glm::vec3(1.0,1.0,0.5));
}
void cuttingController::update(){
    if(p_p2v_dirty){
        p_p2v_dirty = false;
        p_p2v_mat = glm::translate(glm::mat4(1.0), p_point_)*p_rotate_mat_ * glm::scale(glm::mat4(1.0), p_scale);
    }
    mTarget tar = mTarget((int)vrController::param_value_map["mtarget"]);
    if(tar == PLANE && vrController::param_bool_map["pfview"]){//keep it static
        p_rotate_mat_ = glm::inverse(vrController::ModelMat_) * p_p2w_mat;
        p_norm_ = rotateNormal(p_rotate_mat_, glm::vec3(.0,.0,1.0));
        p_p2v_dirty = true;
    }
    else{
        p_p2w_mat = vrController::ModelMat_ * p_p2v_mat;
    }
}

void cuttingController::draw_plane(){
    if(!pshader){
        pshader = new Shader();
        if(!pshader->Create("shaders/cplane.vert", "shaders/cplane.frag"))
            LOGE("Raycast===Failed to create cutting plane shader program===");
    }
    pshader->Use();
    pshader->setMat4("uMVP", vrController::camera->getVPMat()* p_p2w_mat);
    pshader->setVec4("uBaseColor", plane_color_);
    if (!pVAO_) {
        float vertices[] = {
                1.0f,1.0f,.0f,
                -1.0f,1.0f,.0f,
                -1.0f,-1.0f,.0f,

                -1.0f,-1.0f,.0f,
                1.0f,-1.0f,.0f,
                1.0f,1.0f,.0f,
        };
        unsigned int VBO = 0;
        glGenVertexArrays(1, &pVAO_);
        glGenBuffers(1, &VBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(pVAO_);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(pVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    pshader->unUse();
}

void cuttingController::setCutPlane(float percent){
    p_point_ = p_start_ + p_norm_* percent * 1.75f;
    p_p2v_dirty = true;
}
void cuttingController::setCutPlane(glm::vec3 normal){}
void cuttingController::setCutPlane(glm::vec3 startPoint, glm::vec3 normal){}
void cuttingController::onRotate(mTarget target, float offx, float offy){
    if(target == PLANE){
        p_rotate_mat_ = glm::rotate(glm::mat4(1.0f), offx, glm::vec3(0,1,0))
                        * glm::rotate(glm::mat4(1.0f), offy, glm::vec3(1,0,0))
                        * p_rotate_mat_;

        p_norm_ = rotateNormal(p_rotate_mat_, glm::vec3(.0f, .0f, 1.0f));
        p_p2v_dirty = true;
    }
}

void cuttingController::onScale(mTarget target, float sx, float sy, float sz){
//    if(sy < .0f)    p_scale = p_scale * sx;
//    else p_scale = p_scale * glm::vec3(sx, sy, sz);
//    p_p2v_dirty = true;
}
void cuttingController::onTranslate(mTarget target, float offx, float offy){
    //do nothing currently
}
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
    mat4 vm_inv = transpose(inverse(vrController::ModelMat_));
    update_plane_(vec3MatNorm(vm_inv, vrController::camera->getViewDirection()));

    p_point_ = p_start_;
    p_point_world = glm::vec3(vrController::ModelMat_ * glm::vec4(p_point_,1.0f));

    update_modelMat_o();

    _mptr = this;
}
cuttingController::cuttingController(glm::vec3 ps, glm::vec3 pn):
p_start_(ps), p_norm_(pn){
    p_point_ = p_start_;
    p_rotate_mat_ = rotMatFromDir(pn);
    update_modelMat_o();
    _mptr = this;
}
void cuttingController::UpdateAndDraw(){
    Update();
    if(vrController::param_bool_map["cutting"]) draw_plane();
}
void cuttingController::setCuttingParams(GLuint sp,bool includePoints){
//    Shader::Uniform(sp,"uSphere.center", glm::vec3(vrController::csphere_c));
//    Shader::Uniform(sp,"uSphere.radius", vrController::csphere_radius);
    Shader::Uniform(sp,"uPlane.p", p_point_);
    Shader::Uniform(sp,"uPlane.normal", p_norm_);//* glm::vec3(1.0,1.0,0.5));
    if(includePoints){
        mat4 p2m_mat = inverse(vrController::ModelMat_)*p_p2w_mat;
        vec3 pms[3];int i=0;
        for(vec4 p: P_Points)
            pms[i++] = vec3(p2m_mat * p);

        Shader::Uniform(sp,"uPlane.s1", pms[0]);
        Shader::Uniform(sp,"uPlane.s2", pms[1]);
        Shader::Uniform(sp,"uPlane.s3", pms[2]);
        Shader::Uniform(sp, "u_plane_color", plane_color_);
    }
}
void cuttingController::Update(){
    update_modelMat_o();
//    if(keep_cutting_position()){//keep it static
//        p_p2o_mat = glm::inverse(vrController::ModelMat_) * p_p2w_mat;
//        p_norm_ = rotateNormal(p_p2o_mat, glm::vec3(.0,.0,1.0));
//        p_point_ = glm::vec3(glm::inverse(vrController::ModelMat_) * glm::vec4(p_point_world,1.0f));
//        p_rotate_mat_ = inverse(scale(mat4(1.0), p_scale)) * translate(mat4(1.0), -p_point_)*p_p2o_mat;
//    }
//    else{
        p_point_world = glm::vec3(vrController::ModelMat_ * glm::vec4(p_point_,1.0f));
        p_p2w_mat = vrController::ModelMat_ * p_p2o_mat;
//    }
}

void cuttingController::draw_plane(){
    if(!pshader){
        pshader = new Shader();
        if(!pshader->AddShaderFile(GL_VERTEX_SHADER,"shaders/cplane.vert")
           ||!pshader->AddShaderFile(GL_FRAGMENT_SHADER,  "shaders/cplane.frag")
           ||!pshader->CompileAndLink())
            LOGE("Raycast===Failed to create cutting plane shader program===");
    }
    GLuint sp = pshader->Use();
    Shader::Uniform(sp,"uMVP", vrController::camera->getVPMat()* p_p2w_mat);
    Shader::Uniform(sp,"uBaseColor", plane_color_);
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
    pshader->UnUse();
}

void cuttingController::setCutPlane(float percent){
//    if(keep_cutting_position()) return;
    p_point_ = p_start_ + p_norm_* percent * 1.75f;
    p_p2o_dirty = true;
}
bool cuttingController::keep_cutting_position(){
    mTarget tar = mTarget((int)vrController::param_value_map["mtarget"]);
    return (tar == PLANE && vrController::param_bool_map["pfview"]);
}
void cuttingController::setCutPlane(glm::vec3 normal){}
void cuttingController::setCutPlane(glm::vec3 startPoint, glm::vec3 normal){}
void cuttingController::onRotate(mTarget target, float offx, float offy){
    if(target == PLANE){
        p_rotate_mat_ = glm::rotate(glm::mat4(1.0f), offx, glm::vec3(0,1,0))
                        * glm::rotate(glm::mat4(1.0f), offy, glm::vec3(1,0,0))
                        * p_rotate_mat_;

        p_norm_ = rotateNormal(p_rotate_mat_, glm::vec3(.0f, .0f, 1.0f));
        p_p2o_dirty = true;
    }
}

void cuttingController::onScale(mTarget target, float sx, float sy, float sz){
    if(sy < .0f)    p_scale = p_scale * sx;
    else p_scale = p_scale * glm::vec3(sx, sy, sz);
    p_p2o_dirty = true;
}
void cuttingController::onTranslate(mTarget target, float offx, float offy){
    //do nothing currently
}
void cuttingController::update_modelMat_o(){
    if(!p_p2o_dirty) return;
    p_p2o_mat = glm::translate(glm::mat4(1.0), p_point_)*p_rotate_mat_ * glm::scale(glm::mat4(1.0), p_scale);
    p_p2o_dirty = false;
}
void cuttingController::update_plane_(glm::mat4 rotMat){
}
void cuttingController::update_plane_(glm::vec3 pNorm){
    p_norm_ = pNorm;
    mat4 vm_inv = transpose(inverse(vrController::ModelMat_));
    p_rotate_mat_ = rotMatFromDir(pNorm);
    glm::vec3 vp_obj = vec3MatNorm(vm_inv, vrController::camera->getCameraPosition());
    //cloest point
    p_start_ = cloestVertexToPlane(pNorm, vp_obj);
}
#include "cuttingController.h"

#include <glm/gtx/transform.hpp>
#include <AndroidUtils/AndroidHelper.h>
#include <vrController.h>
#include <glm/gtx/rotate_vector.hpp>

cuttingController::cuttingController(){
    p_start_ = glm::vec3(.0f);p_point_ = glm::vec3(.0f); p_norm_=glm::vec3(1.0, .0, .0f);
}
cuttingController::cuttingController(glm::vec3 start_p, glm::vec3 normal)
:p_start_(start_p), p_point_(p_start_){//default percent = 0
    p_norm_ = glm::normalize(normal);
}
void cuttingController::Draw(){
    if(vrController::param_bool_map["cutting"])DrawPlane();

}
void cuttingController::setCuttingParams(Shader* shader){
    shader->setVec3("uSphere.center", glm::vec3(vrController::csphere_c));
    shader->setFloat("uSphere.radius", vrController::csphere_radius);

    shader->setVec3("uPlane.p", p_point_);
    shader->setVec3("uPlane.normal", p_norm_);
}
void cuttingController::DrawPlane(){
    if(!pshader){
        pshader = new Shader();
        if(!pshader->Create("shaders/cplane.vert", "shaders/cplane.frag"))
            LOGE("Raycast===Failed to create cutting plane shader program===");
    }

    pshader->Use();

    glm::mat4 model_mat =
            glm::translate(glm::mat4(1.0), p_point_ + vrController::PosVec3_)*
            vrController::RotateMat_* glm::orientation(p_norm_, glm::vec3(0,0,1))*
            glm::scale(glm::mat4(1.0), glm::vec3(1.0) * vrController::ScaleVec3_);
    pshader->setMat4("uMVP", vrController::camera->getProjMat() * vrController::camera->getViewMat()*model_mat);
    pshader->setVec4("uBaseColor", glm::vec4(0.2f, .0f, .0f, 1.0f));

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
void cuttingController::DrawSphere(){

}
void cuttingController::setCutPlane(float percent){
    p_point_ = p_start_ + p_norm_* percent * 1.75f;
}
void cuttingController::setCutPlane(glm::vec3 normal){}
void cuttingController::setCutPlane(glm::vec3 startPoint, glm::vec3 normal){}
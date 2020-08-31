#include "cuttingController.h"

#include <glm/gtx/transform.hpp>
#include <vrController.h>
#include <glm/gtx/rotate_vector.hpp>
#include <Utils/mathUtils.h>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

cuttingController* cuttingController::_mptr = nullptr;
cuttingController* cuttingController::instance(){
    if(!_mptr)_mptr = new cuttingController;
    return _mptr;
}
cuttingController::cuttingController(){
    auto model_mat = vrController::instance()->getModelMatrix();
    mat4 vm_inv = transpose(inverse(model_mat));
    update_plane_(vec3MatNorm(vm_inv, Manager::camera->getViewDirection()));
    onReset();
    _mptr = this;
}
cuttingController::cuttingController(glm::vec3 ps, glm::vec3 pn):
p_start_(ps), p_norm_(pn){
    p_rotate_mat_ = rotMatFromDir(pn);
    onReset();
    _mptr = this;
}
void cuttingController::onReset(){
    p_point_ = p_start_;
    // p_point_world = glm::vec3(model_mat* glm::vec4(p_point_,1.0f));
    update_modelMat_o();
    p_scale = glm::vec3(1.0);
    rc.point=p_point_;rc.scale=p_scale;rc.rotate_mat=p_rotate_mat_;rc.move_value=cmove_value;
}
void cuttingController::Update(){
    update_modelMat_o();
    auto model_mat = vrController::instance()->getModelMatrix(true);

    if(keep_cutting_position()){//keep it static
        p_p2o_mat = glm::inverse(model_mat) * p_p2w_mat;
        update_plane_(rotateNormal(p_p2o_mat, glm::vec3(.0,.0,1.0)));
        p_point_ = glm::vec3(glm::inverse(model_mat) * glm::vec4(p_point_world,1.0f));
        if(cmove_value){
            p_point_ += cmove_value * p_norm_;
            p_p2o_mat = glm::translate(glm::mat4(1.0), cmove_value * p_norm_)*p_p2o_mat;
            p_p2w_mat = model_mat * p_p2o_mat;
            p_point_world = glm::vec3(model_mat * glm::vec4(p_point_,1.0f));
        }
    }
    else{
        p_point_world = glm::vec3(model_mat * glm::vec4(p_point_,1.0f));
        p_p2w_mat = model_mat * p_p2o_mat;
    }
}
void cuttingController::Draw(){
    if(Manager::param_bool[dvr::CHECK_CUTTING] || Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]) draw_plane();
}
void cuttingController::UpdateAndDraw(){
    Update();
    if(Manager::param_bool[dvr::CHECK_CUTTING] || Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]) draw_plane();
}
void cuttingController::setCuttingParams(GLuint sp, bool includePoints){
//    Shader::Uniform(sp,"uSphere.center", glm::vec3(vrController::csphere_c));
//    Shader::Uniform(sp,"uSphere.radius", vrController::csphere_radius);
    Shader::Uniform(sp,"uPlane.p", p_point_);
    Shader::Uniform(sp,"uPlane.normal", p_norm_);//* glm::vec3(1.0,1.0,0.5));
    if(includePoints){
        // mat4 p2m_mat = inverse(vrController::instance()->getModelMatrix())*p_p2w_mat;
        vec3 pms[3];int i=0;
        for(vec4 p: P_Points)
            pms[i++] = vec3(p_p2o_mat * p);

        Shader::Uniform(sp,"uPlane.s1", pms[0]);
        Shader::Uniform(sp,"uPlane.s2", pms[1]);
        Shader::Uniform(sp,"uPlane.s3", pms[2]);
        Shader::Uniform(sp, "u_plane_color", plane_color_);
    }
}

void cuttingController::draw_plane(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    if(!pshader){
        pshader = new Shader();
        if(!pshader->AddShader(GL_VERTEX_SHADER,Manager::shader_contents[dvr::SHADER_CPLANE_VERT])
           ||!pshader->AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_CPLANE_FRAG])
           ||!pshader->CompileAndLink())
            LOGE("Raycast===Failed to create cutting plane shader program===");
        // Manager::shader_contents[dvr::SHADER_CPLANE_VERT] = "";Manager::shader_contents[dvr::SHADER_CPLANE_FRAG]="";

    }
    GLuint sp = pshader->Use();

    Shader::Uniform(sp,"uMVP", Manager::camera->getVPMat()* p_p2w_mat);
    
    Shader::Uniform(sp,"uBaseColor", plane_color_);
    if (!pVAO_) {
        float vertices[] = {
                0.5f,0.5f,.0f,
                -0.5f,0.5f,.0f,
                -0.5f,-0.5f,.0f,

                -0.5f,-0.5f,.0f,
                0.5f,-0.5f,.0f,
                0.5f,0.5f,.0f,
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
        glDisable(GL_BLEND);
    // glDisable(GL_DEPTH_TEST);
}

void cuttingController::setCutPlane(float value){
    if(keep_cutting_position()) {cmove_value =  value * CUTTING_FACTOR; return;}
    p_point_ += p_norm_ * value * CUTTING_FACTOR;
    cmove_value = .0f;
    p_p2o_dirty = true;
}
bool cuttingController::keep_cutting_position(){
    return Manager::param_bool[dvr::CHECK_FREEZE_CPLANE];
}
void cuttingController::setCutPlane(glm::vec3 normal){}
void cuttingController::setCutPlane(glm::vec3 pp, glm::vec3 normal){
    p_norm_ = glm::normalize(normal); p_point_=pp;

    p_rotate_mat_ = rotMatFromDir(p_norm_);
    cmove_value = .0f;
    p_p2o_dirty = true;
}
void cuttingController::setCuttingPlaneDelta(int delta){
    float value = delta * CMOVE_UNIT_SIZE;
    if(keep_cutting_position()) {cmove_value =  value; return;}
    p_point_ += p_norm_ * value;
    cmove_value = .0f;
    p_p2o_dirty = true;
}
float* cuttingController::getCutPlane(){
    float* data = new float[6];
    memcpy(data, glm::value_ptr(p_point_), 3* sizeof(float));
    memcpy(data+3, glm::value_ptr(p_norm_), 3* sizeof(float));
    return data;
}

void cuttingController::onRotate(float offx, float offy){
    update_plane_(glm::rotate(glm::mat4(1.0f), offx, glm::vec3(0,1,0))
                    * glm::rotate(glm::mat4(1.0f), offy, glm::vec3(1,0,0))
                    * p_rotate_mat_);
    p_p2o_dirty = true;
}

void cuttingController::onScale(float sx, float sy, float sz){
    if(sy < .0f) p_scale = p_scale * sx;
    else p_scale = p_scale * glm::vec3(sx, sy, sz);
    p_p2o_dirty = true;
}
void cuttingController::onTranslate(float offx, float offy){
    //do nothing currently
}
void cuttingController::update_modelMat_o(){
    if(!p_p2o_dirty) return;
    p_p2o_mat = glm::translate(glm::mat4(1.0), p_point_)* p_rotate_mat_ * glm::scale(glm::mat4(1.0), p_scale);
    p_p2o_dirty = false;
}
void cuttingController::update_plane_(glm::mat4 rotMat){
    p_rotate_mat_ = rotMat;
    p_norm_ = rotateNormal(p_rotate_mat_, glm::vec3(.0f, .0f, 1.0f));
//    mat4 vm_inv = transpose(inverse(vrController::ModelMat_));
//    glm::vec3 vp_obj = vec3MatNorm(vm_inv, vrController::camera->getCameraPosition());
//    //cloest point
//    p_start_ = cloestVertexToPlane(p_norm_, vp_obj);
    //todo: percent of how p_point account for the percent with ro, rd
}
void cuttingController::update_plane_(glm::vec3 pNorm){
    p_norm_ = pNorm;
    mat4 vm_inv = transpose(inverse(vrController::instance()->getModelMatrix()));
    p_rotate_mat_ = rotMatFromDir(pNorm);
    glm::vec3 vp_obj = vec3MatNorm(vm_inv, Manager::camera->getCameraPosition());
    //cloest point
    // p_start_ = cloestVertexToPlane(pNorm, vp_obj) - p_norm_*0.1f;
    //debug
    p_start_ = cloestVertexToPlane(pNorm, vp_obj) + p_norm_*0.5f;
}
void cuttingController::SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id){
    if(cut_plane_id == dvr::CUT_CUTTING_PLANE && last_mode==dvr::CUT_TRAVERSAL){
        rt.point=p_point_;rt.scale=p_scale;rt.rotate_mat=p_rotate_mat_;rt.move_value=cmove_value;
        p_scale = rc.scale;
        // setCutPlane(rc.point,rc.norm);
        p_point_ = rc.point;
        cmove_value = rc.move_value;
        p_p2o_dirty = true;
        update_plane_(rc.rotate_mat);

        last_mode = dvr::CUT_CUTTING_PLANE;
    }else if(cut_plane_id == dvr::CUT_TRAVERSAL && last_mode == dvr::CUT_CUTTING_PLANE){
        rc.point=p_point_;rc.scale=p_scale;rc.rotate_mat=p_rotate_mat_;rc.move_value=cmove_value;
        p_scale = rt.scale;
        // setCutPlane(rt.point,rt.norm);
        p_point_ = rt.point;
        cmove_value = rt.move_value;
        p_p2o_dirty = true;
        update_plane_(rt.rotate_mat);

        last_mode = dvr::CUT_TRAVERSAL;
    }
}
void cuttingController::set_centerline_cutting(int& id, glm::vec3& pp, glm::vec3& pn){
	id = fmax(id, center_sample_gap);
	id = fmin(id,3999-center_sample_gap);

	float * data = pmap[dvr::ORGAN_COLON];
	pp = glm::vec3(data[3*id],data[3*id+1],data[3*id+2]);
	pn = glm::vec3(data[3*(id+center_sample_gap)], data[3*(id+center_sample_gap)+1], data[3*(id+center_sample_gap)+2]) 
        - glm::vec3(data[3*(id-center_sample_gap)], data[3*(id-center_sample_gap)+1], data[3*(id-center_sample_gap)+2]);
	if(glm::dot(pn,glm::vec3(0,0,-1)) < .0f) pn=-pn;
	// setCutPlane(pp, pn);
}
void cuttingController::setupCenterLine(dvr::ORGAN_IDS id, float* data){
    pmap[id] = data;
    if(id == dvr::ORGAN_COLON){
        clp_id_ = 0;
        glm::vec3 pp, pn;
        set_centerline_cutting(clp_id_,pp,pn);
        rt.point=pp;rt.scale=glm::vec3(0.1f);rt.rotate_mat= rotMatFromDir(pn);rt.move_value=.0f;
    }
}
void cuttingController::setCenterLinePos(int id, int delta_id){
    if(delta_id == 0){
        clp_id_ = id%4000;
    }else{
        clp_id_=(clp_id_+delta_id)%4000;
    }
    set_centerline_cutting(clp_id_, p_point_, p_norm_);
    setCutPlane(p_point_, p_norm_);
}

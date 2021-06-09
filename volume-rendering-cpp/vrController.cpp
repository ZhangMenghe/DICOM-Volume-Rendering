#include "vrController.h"
#include <Utils/mathUtils.h>
#include <dicomRenderer/screenQuad.h>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
using namespace dvr;
using namespace glm;
vrController* vrController::myPtr_ = nullptr;

vrController* vrController::instance(){
    return myPtr_;
}
vrController::~vrController(){
    if(texvrRenderer_) delete texvrRenderer_;
    if(raycastRenderer_) delete raycastRenderer_;
    // if(meshRenderer_) delete meshRenderer_;
    for(auto inst:line_renderers_)delete inst.second;
    line_renderers_.clear();
    if(bakeShader_) delete bakeShader_;
    if(tex_volume) delete tex_volume;
    if(tex_baked) delete tex_baked;
}
vrController::vrController(const std::shared_ptr<Manager> &manager)
:m_manager(manager){
//    onReset();
    myPtr_ = this;
}
void vrController::onReset() {
    Mouse_old = glm::fvec2(.0);
    if(m_manager->addMVPStatus("default_status", true)){
        m_manager->getCurrentMVPStatus(RotateMat_, ScaleVec3_, PosVec3_);
        volume_model_dirty = true;
    }
    if(cutter_) cutter_->onReset();
}
void vrController::onReset(glm::vec3 pv, glm::vec3 sv, glm::mat4 rm, Camera* cam){
    Mouse_old = glm::fvec2(.0f);
//    glm::mat4 mm =  glm::translate(glm::mat4(1.0), pv)
//                 * rm
//                 * glm::scale(glm::mat4(1.0), sv);
    if(m_manager->addMVPStatus("template", rm, sv, pv, cam, true)){
        m_manager->getCurrentMVPStatus(RotateMat_, ScaleVec3_, PosVec3_);
        volume_model_dirty = true;
    }

    if(cutter_) cutter_->onReset();
}

void vrController::assembleTexture(int update_target, int ph, int pw, int pd, float sh, float sw, float sd, GLubyte * data, int channel_num){
    if(update_target==0 || update_target==2){
        vol_dimension_ = glm::vec3(ph,pw,pd);
        if(sh<=0 || sw<=0 || sd<=0){
            if(pd > 200) vol_dim_scale_ = glm::vec3(1.0f, 1.0f, 0.5f);
            else if(pd > 100) vol_dim_scale_ = glm::vec3(1.0f, 1.0f, pd / 300.f);
            else vol_dim_scale_ = glm::vec3(1.0f, 1.0f, pd / 200.f);
        }else if(abs(sh - sw) < 1){
             vol_dim_scale_ = glm::vec3(1.0f, 1.0f, sd / sh);
        }else{
             vol_dim_scale_ = glm::vec3(1.0f, 1.0f, sd / fmax(sw,sh));
            if(sw > sh){
                ScaleVec3_ = ScaleVec3_* glm::vec3(1.0, sh / sw, 1.0);
            }else{
                ScaleVec3_ = ScaleVec3_* glm::vec3(sw/sh, 1.0, 1.0);
            }
            volume_model_dirty = true;
        }
        vol_dim_scale_mat_=glm::scale(glm::mat4(1.0f), vol_dim_scale_);
        m_manager->setDimension(vol_dimension_);
        texvrRenderer_->setDimension(vol_dimension_, vol_dim_scale_);
        cutter_->setDimension(pd, vol_dim_scale_.z);
    }
    auto vsize= ph*pw*pd;
    uint32_t* vol_data  = new uint32_t[vsize];
    uint16_t tm;
    //fuse volume data
    for(auto i=0, shift = 0; i<vsize; i++, shift+=channel_num) {
        vol_data[i] = uint32_t((((uint32_t)data[shift+1])<<8) + (uint32_t)data[shift]);
        tm = (channel_num==4)?uint16_t((((uint16_t)data[shift+3])<<8)+data[shift+2]):(uint16_t)0;
        vol_data[i] = uint32_t((((uint32_t)tm)<<16)+vol_data[i]);
        // vol_data[i] = (((uint32_t)data[4*i+3])<<24)+(((uint32_t)data[4*i+3])<<16)+(((uint32_t)data[4*i+1])<<8) + ((uint32_t)data[4*i]);
    }
    if(tex_volume!= nullptr){delete tex_volume; tex_volume= nullptr;}
    tex_volume = new Texture(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, pw, ph, pd, vol_data);

    auto* tb_data = new GLubyte[vsize * 4];
    if(tex_baked!= nullptr){delete tex_baked; tex_baked= nullptr;}
    tex_baked = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pw, ph, pd, tb_data);
    delete[]tb_data;
    delete[]vol_data;
    Manager::baked_dirty_ = true;
    meshRenderer_->Setup(ph,pw,pd);
}
void vrController::setupCenterLine(int id, float* data){
    int oid = 0;
    while(id/=2)oid++;
    line_renderers_[oid] = new centerLineRenderer(oid);
    // for(int i=0;i<4000;i++){
    //     data[3*i] = i / 4000.0f - 0.5;
    //     data[3*i+1] = i / 4000.0f - 0.5;
    //     data[3*i+2] = .0f;

    // }
    line_renderers_[oid]->updateVertices(4000, data);
    cutter_->setupCenterLine((dvr::ORGAN_IDS)oid, data);
}

void vrController::onViewCreated(){
    texvrRenderer_ = new texvrRenderer();
    raycastRenderer_ = new raycastRenderer();
    meshRenderer_ = new organMeshRenderer;
    cutter_ = new cuttingController;
    data_board_ = new dataBoard;
}

void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    data_board_->onViewChange(width, height);
    screenQuad::instance()->onScreenSizeChange(width, height);
}

bool vrController::check_ar_ray_intersect(){
    Camera* cam = Manager::camera;
    glm::mat4 model_inv = glm::inverse(ModelMat_ * vol_dim_scale_mat_);
    glm::vec3 ro = glm::vec3(model_inv*glm::vec4(Manager::camera->getCameraPosition(), 1.0));
    if(!ray_initialized){
        float tangent = tan(Manager::camera->getFOV() * 0.5f);
        vec2 ts = screenQuad::instance()->getTexSize();
        float ar = ts.x / ts.y;
        float u = (ts.x *0.5f + 0.5f)/ts.x * 2.0f -1.0f;
        float v = (ts.y * 0.5f + 0.5f)/ts.y * 2.0f -1.0f;
        ray_dir = vec4(u* tangent*ar, v*tangent, -1.0, .0);
        ray_initialized = true;
    }

    glm::vec3 rd = glm::vec3(glm::normalize(model_inv * Manager::camera->getCameraPose() *ray_dir));
    vec3 extents = vec3(.5f);

    vec3 tMin = (-extents - ro) / rd;
    vec3 tMax = (extents - ro) / rd;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    vec2 res =  vec2(max(max(t1.x, t1.y), t1.z), min(min(t2.x, t2.y), t2.z));
    return res.x<res.y;
}

void vrController::onDrawScene(){
    if(!tex_volume) return;
    if(Manager::mvp_dirty_){m_manager->getCurrentMVPStatus(RotateMat_, ScaleVec3_, PosVec3_); volume_model_dirty=true;}
    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(Manager::volume_ar_hold){
        vec3 view_dir = glm::normalize(Manager::camera->getViewDirection());
        PosVec3_ = Manager::camera->getCameraPosition()+ view_dir;
        RotateMat_ = Manager::camera->getRotationMatrixOfCameraDirection();
        updateVolumeModelMat();
    }
    glm::mat4 model_mat = ModelMat_ * vol_dim_scale_mat_;
    bool cp_update = Manager::IsCuttingNeedUpdate();
    bool draw_finished =false;
    if(cp_update){
        cutter_->Update();
        if(Manager::param_bool[dvr::CHECK_CUTTING]){
            if(isRayCasting() && pre_draw_)draw_finished = true;
            else{
                cutter_->Draw(pre_draw_);
                draw_finished=true;
            }
        }
    }

    precompute();

    //volume
    if(Manager::isDrawVolume()){
        if(isRayCasting())  raycastRenderer_->Draw(pre_draw_, model_mat);
        else texvrRenderer_->Draw(pre_draw_, ModelMat_);
    }

    //mesh
    if(Manager::isDrawMesh()) meshRenderer_->Draw(pre_draw_, model_mat);

    //centerline
    if(Manager::isDrawCenterLine()){
        auto mask_bits = Manager::getMaskBits();
        for(auto line:line_renderers_)
            if((mask_bits>> (line.first+1)) & 1)line.second->onDraw(pre_draw_, model_mat);
    }

    if(cp_update&&!draw_finished)cutter_->Draw(pre_draw_);

    //data board
    if(Manager::param_bool[dvr::CHECK_OVERLAY])
        data_board_->onDraw(pre_draw_);

    Manager::baked_dirty_ = false;
    //  LOGE("===FPS: %.2f==\n", pm_.Update());
}
void vrController::onDraw() {
    if(!tex_volume) return;
    glClearColor(.0f,.0f,.0f,.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(!pre_draw_){onDrawScene();return;}

    //for pre-draw option
    if(isDirty()){
        screenQuad::instance()->Clear();
        onDrawScene();
    }
    screenQuad::instance()->Draw();
}
void vrController::onSingleTouchDown(float x, float y){
    Mouse_old = glm::fvec2(x, y);
}
void vrController::onTouchMove(float x, float y) {
    if(!tex_volume) return;

    if(!Manager::param_bool[dvr::CHECK_CUTTING]&&Manager::param_bool[dvr::CHECK_FREEZE_VOLUME]) return;

    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();

    float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
    Mouse_old = glm::fvec2(x, y);
    xoffset *= MOUSE_ROTATE_SENSITIVITY;
    yoffset *= -MOUSE_ROTATE_SENSITIVITY;

    if(Manager::param_bool[dvr::CHECK_FREEZE_VOLUME]){
        cutter_->onRotate(xoffset, yoffset);
        return;
    }

    RotateMat_ = mouseRotateMat(RotateMat_, xoffset, yoffset);
    volume_model_dirty = true;
}
void vrController::onScale(float sx, float sy){
    if(!tex_volume) return;
    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
    //unified scaling
    if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
    else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;

    if(Manager::param_bool[dvr::CHECK_FREEZE_VOLUME]){
        cuttingController::instance()->onScale(sx);
    }else{
        ScaleVec3_ = ScaleVec3_* sx;
        volume_model_dirty = true;
    }
}
void vrController::onPan(float x, float y){
    if(!tex_volume|| Manager::param_bool[dvr::CHECK_FREEZE_VOLUME]) return;
    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();

    float offx = x / Manager::screen_w* MOUSE_PAN_SENSITIVITY, offy = -y /Manager::screen_h*MOUSE_PAN_SENSITIVITY;
    PosVec3_.x += offx * ScaleVec3_.x;
    PosVec3_.y += offy * ScaleVec3_.y;
    volume_model_dirty = true;
}
void vrController::precompute(){
    if(!Manager::baked_dirty_) return;
    if(!bakeShader_){
        //geometry program
        bakeShader_ = new Shader;
        if(!bakeShader_->AddShader(GL_COMPUTE_SHADER, Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_GLSL])
           ||!bakeShader_->CompileAndLink())
            LOGE("Raycast=====Failed to create geometry shader");
        Manager::shader_contents[dvr::SHADER_RAYCASTVOLUME_GLSL]="";

    }
    bakeShader_->DisableAllKeyword();
    bakeShader_->EnableKeyword(Manager::instance()->getColorSchemeKeyword());

    //todo!!!! add flip stuff
    bakeShader_->EnableKeyword("FLIPY");

    if(m_use_raw_data)bakeShader_->EnableKeyword("RAW_DATA");
    else bakeShader_->DisableKeyword("RAW_DATA");

    if(Manager::param_bool[dvr::CHECK_MASKON]) bakeShader_->EnableKeyword("SHOW_ORGANS");
    else bakeShader_->DisableKeyword("SHOW_ORGANS");

    GLuint sp = bakeShader_->Use();
    glBindImageTexture(0, tex_volume->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(1, tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    m_manager->updateVolumeSetupUniforms(sp);

    glDispatchCompute((GLuint)(tex_volume->Width() + 7) / 8, (GLuint)(tex_volume->Height() + 7) / 8, (GLuint)(tex_volume->Depth() + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);//GL_RGBA16UI);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    bakeShader_->UnUse();

    data_board_->onUpdate(m_manager.get());
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}

void vrController::setShaderContents(dvr::SHADER_FILES fid, std::string content){
    if(fid < dvr::SHADER_END)
        Manager::shader_contents[fid] = content;
}

void vrController::setVolumeRST(glm::mat4 rm, glm::vec3 sv, glm::vec3 pv){
    RotateMat_=rm; ScaleVec3_=sv; PosVec3_=pv;
    volume_model_dirty=true;
}

void vrController::updateVolumeModelMat(){
    ModelMat_ =
            glm::translate(glm::mat4(1.0), PosVec3_)
                 * RotateMat_
                 * glm::scale(glm::mat4(1.0), ScaleVec3_);
    if(Manager::isARWithMarker()){
        float* pData = glm::value_ptr(ModelMat_);
        for(int i=0;i<16;i++) pData[i]*=m_inverse_[i];
    }
}



void vrController::setCuttingPlane(float value){
    if(Manager::param_bool[dvr::CHECK_CUTTING] && !isRayCasting()) {
        cutter_->setCutPlane(value);
        texvrRenderer_->setCuttingPlane(value);
        if(pre_draw_)texvrRenderer_->dirtyPrecompute();
    }else if( Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]){
        if(!cutter_->IsCenterLineAvailable())return;
        cutter_->setCenterLinePos((int)(value * 4000.0f));
        if(Manager::param_bool[dvr::CHECK_TRAVERSAL_VIEW]) {
            AlignModelMatToTraversalPlane();
            isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
        }
    }
}
void vrController::setCuttingPlane(int id, int delta){
    if(Manager::param_bool[dvr::CHECK_CUTTING]){
        if(isRayCasting()) {cutter_->setCuttingPlaneDelta(delta);raycastRenderer_->dirtyPrecompute();}
        else texvrRenderer_->setCuttingPlaneDelta(delta);
    }else if( Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]){
        if(!cutter_->IsCenterLineAvailable())return;
        cutter_->setCenterLinePos(id, delta);
        if(Manager::param_bool[dvr::CHECK_TRAVERSAL_VIEW]) {
            AlignModelMatToTraversalPlane();
            isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
        }
    }
}
void vrController::setCuttingPlane(glm::vec3 pp, glm::vec3 pn){
    cutter_->setCutPlane(pp, pn);
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}
float* vrController::getCuttingPlane(){
    return cutter_->getCutPlane();
}
void vrController::setCuttingParams(GLuint sp){
    cutter_->setCuttingParams(sp);
}
void vrController::SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id){
    cutter_->SwitchCuttingPlane(cut_plane_id);
    if(Manager::param_bool[dvr::CHECK_TRAVERSAL_VIEW]) AlignModelMatToTraversalPlane();
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}
void vrController::setOverlayRects(int id, int width, int height, int left, int top){
    data_board_->setOverlayRect(id, width, height, left, top);
}
void vrController::AlignModelMatToTraversalPlane(){
    glm::vec3 pp, pn;
    cutter_->getCurrentTraversalInfo(pp, pn);
    pn = glm::normalize(pn);

    RotateMat_ = glm::toMat4(glm::rotation(pn, glm::vec3(.0,.0,-1.0f)));
                
    volume_model_dirty = true;
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}

float* vrController::getCurrentReservedStates(){
    float* data = new float[31];
    memcpy(data, glm::value_ptr(PosVec3_), 3* sizeof(float));
    memcpy(data+3, glm::value_ptr(ScaleVec3_), 3* sizeof(float));
    memcpy(data+6, glm::value_ptr(RotateMat_), 16* sizeof(float));
    memcpy(data+22, glm::value_ptr(Manager::camera->getCameraPosition()), 3* sizeof(float));
    memcpy(data+25, glm::value_ptr(Manager::camera->getViewUpDirection()), 3* sizeof(float));
    memcpy(data+28, glm::value_ptr(Manager::camera->getViewCenter()), 3* sizeof(float));
    return data;
}
bool vrController::isDirty() {
    if(!tex_volume) return false;
    if(!pre_draw_||volume_model_dirty||Manager::baked_dirty_){
        meshRenderer_->dirtyPrecompute();
        return true;
    }
    if(Manager::IsCuttingNeedUpdate()&&cutter_->isPrecomputeDirty()){
        meshRenderer_->dirtyPrecompute();
        if(isRayCasting())raycastRenderer_->dirtyPrecompute();
        else texvrRenderer_->dirtyPrecompute();
        return true;
    }
    if(Manager::param_bool[dvr::CHECK_VOLUME_ON]){
        if(isRayCasting()) return raycastRenderer_->isPrecomputeDirty();
        return texvrRenderer_->isPrecomputeDirty();
    }
    return false;
}

#include "vrController.h"
#include "overlayController.h"
#include <Utils/mathUtils.h>
#include <dicomRenderer/screenQuad.h>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

using namespace dvr;
vrController* vrController::myPtr_ = nullptr;

vrController* vrController::instance(){
    if(!myPtr_) myPtr_ = new vrController;
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
    rStates_.clear();
}
vrController::vrController(){
    onReset();
    mesh_renders = std::vector<organMeshRenderer*>(dvr::ORGAN_END, nullptr);
    myPtr_ = this;
}
void vrController::onReset() {
    Mouse_old = glm::fvec2(.0);
    rStates_.clear();
    cst_name="";
    setMVPStatus("default_status");
}
void vrController::onReset(glm::vec3 pv, glm::vec3 sv, glm::mat4 rm, Camera* cam){
    rm = glm::mat4(1.0);
    Mouse_old = glm::fvec2(.0f);
    rStates_.clear();
    cst_name="template";
    glm::mat4 mm =  glm::translate(glm::mat4(1.0), pv)
                 * rm
                 * glm::scale(glm::mat4(1.0), sv);
    rStates_[cst_name] = reservedStatus(mm, rm, sv, pv, cam);


    if(Manager::screen_w != 0)rStates_[cst_name].vcam->setProjMat(Manager::screen_w, Manager::screen_h);
    ModelMat_=mm; RotateMat_=rm; ScaleVec3_=sv; PosVec3_=pv; Manager::camera=rStates_[cst_name].vcam;
    volume_model_dirty = false;
}

void vrController::assembleTexture(int update_target, int ph, int pw, int pd, float sh, float sw, float sd, GLubyte * data, int channel_num){
    if(update_target==0 || update_target==2){
        if(sh<=0 || sw<=0 || sd<=0){
            texvrRenderer_->setDimension(pd, -1);
            raycastRenderer_->setDimension(pd, -1);
        }else if(abs(sh - sw) < 1){
            texvrRenderer_->setDimension(pd, sd / sh);
            raycastRenderer_->setDimension(pd, sd / sh);
        }else{
            float ls = fmax(sw,sh);
            texvrRenderer_->setDimension(pd, sd / ls);
            raycastRenderer_->setDimension(pd, sd / ls);
            if(sw > sh){
                ScaleVec3_ = ScaleVec3_* glm::vec3(1.0, sh / sw, 1.0);
            }else{
                ScaleVec3_ = ScaleVec3_* glm::vec3(sw/sh, 1.0, 1.0);
            }
            volume_model_dirty = true;
        }
        cutter_->setDimension(pd);
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
    line_renderers_[oid]->updateVertices(4000, data);
    cutter_->setupCenterLine((dvr::ORGAN_IDS)oid, data);
}

//1-baldder, 2-kidn 4 color 8 spleen
void vrController::onViewCreated(){
    onViewCreated(pre_draw_);
}

void vrController::onViewCreated(bool pre_draw){
    pre_draw_ = pre_draw;
    texvrRenderer_ = new texvrRenderer(pre_draw);
    raycastRenderer_ = new raycastRenderer(pre_draw);
    meshRenderer_ = new organMeshRenderer;
    cutter_ = new cuttingController;
}

void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    screenQuad::instance()->onScreenSizeChange(width, height);
}
void vrController::onDraw() {
    if(!tex_volume) return;
    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    //ORDER REALLY MATTERS!!!!

    cutter_->Update();
    if(Manager::param_bool[dvr::CHECK_CUTTING])cutter_->Draw();
    if(!Manager::param_bool[dvr::CHECK_MASKON] || Manager::param_bool[dvr::CHECK_VOLUME_ON]){
        precompute();
        if(isRayCasting())  raycastRenderer_->Draw();
        else texvrRenderer_->Draw();
    }

    if(Manager::param_bool[dvr::CHECK_MASKON]){
        if(Manager::param_bool[dvr::CHECK_DRAW_POLYGON]) meshRenderer_->Draw();
        //draw centerline
        if(Manager::param_bool[dvr::CHECK_CENTER_LINE]){
            for(auto line:line_renderers_)
                if((mask_bits_>> (line.first+1)) & 1)line.second->onDraw(ModelMat_ * raycastRenderer_->getDimScaleMat());
        }
    }
    if(Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL])cutter_->Draw();
    Manager::baked_dirty_ = false;
    //  LOGE("===FPS: %.2f==\n", pm_.Update());
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
        cuttingController::instance()->onRotate(xoffset, yoffset);
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
    overlayController::instance()->updateUniforms(render_params_);
    bakeShader_->DisableAllKeyword();
    bakeShader_->EnableKeyword(COLOR_SCHEMES[Manager::color_scheme_id]);
    //todo!!!! add flip stuff
    bakeShader_->EnableKeyword("FLIPY");
    if(Manager::param_bool[dvr::CHECK_MASKON]) bakeShader_->EnableKeyword("SHOW_ORGANS");
    else bakeShader_->DisableKeyword("SHOW_ORGANS");

    GLuint sp = bakeShader_->Use();
    glBindImageTexture(0, tex_volume->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(1, tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_tex_size", glm::vec3(float(tex_volume->Width()), float(tex_volume->Height()), float(tex_volume->Depth())));
    Shader::Uniform(sp, "u_maskbits", mask_bits_);
    Shader::Uniform(sp, "u_organ_num", mask_num_);
    Shader::Uniform(sp, "u_mask_color", Manager::param_bool[CHECK_MASK_RECOLOR]);

    float* widget_data_pointer;
    int widget_num;
    overlayController::instance()->getWidgetFlatPoints(widget_data_pointer, widget_num);
    auto visibles = overlayController::instance()->getWidgetVisibilities();
    int visible=0;
    for(int i=0;i<widget_num;i++){visible |= int(visibles[i]) << i;}
    Shader::Uniform(sp, "u_visible_bits", visible);
    Shader::Uniform(sp, "u_opacity", 6*widget_num, widget_data_pointer);
    Shader::Uniform(sp, "u_widget_num", widget_num);

    Shader::Uniform(sp, "u_contrast_low", render_params_[RENDER_CONTRAST_LOW]);
    Shader::Uniform(sp, "u_contrast_high", render_params_[RENDER_CONTRAST_HIGH]);
//    Shader::Uniform(sp, "u_contrast_level", render_params_[RENDER_CONTRAST_LEVEL]);
    Shader::Uniform(sp, "u_brightness", render_params_[RENDER_BRIGHTNESS]);

    glDispatchCompute((GLuint)(tex_volume->Width() + 7) / 8, (GLuint)(tex_volume->Height() + 7) / 8, (GLuint)(tex_volume->Depth() + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);//GL_RGBA16UI);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    bakeShader_->UnUse();
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}

void vrController::setShaderContents(dvr::SHADER_FILES fid, std::string content){
    if(fid < dvr::SHADER_END)
        Manager::shader_contents[fid] = content;
}
void vrController::updateVolumeModelMat(){
    ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
                 * RotateMat_
                 * glm::scale(glm::mat4(1.0), ScaleVec3_);
}
void vrController::setMVPStatus(std::string status_name){
    //save changes to current status
    if(status_name == cst_name) return;
    if(!cst_name.empty()) rStates_[cst_name] = reservedStatus(ModelMat_, RotateMat_, ScaleVec3_, PosVec3_, Manager::camera);

    //restore / create status
    auto it = rStates_.find(status_name);
    if (it == rStates_.end()) {
        rStates_[status_name] = reservedStatus();
        //LOGE("===create status for %s\n", status_name.c_str());
        // for debug camera only
        //if(status_name == "Raycasting") rStates_[status_name].vcam->setPosition();
        if(Manager::screen_w != 0)rStates_[status_name].vcam->setProjMat(Manager::screen_w, Manager::screen_h);
    }

    auto rstate_ = rStates_[status_name];
    ModelMat_=rstate_.model_mat; RotateMat_=rstate_.rot_mat; ScaleVec3_=rstate_.scale_vec; PosVec3_=rstate_.pos_vec; Manager::camera=rstate_.vcam;

    volume_model_dirty = false;
    cst_name = status_name;
//    auto cpos= camera->getCameraPosition();
//    LOGE("===current status %s, pos: %f, %f, %f, camera: %f, %f, %f", cst_name.c_str(), PosVec3_.x, PosVec3_.y, PosVec3_.z, cpos.x, cpos.y, cpos.z);
}
void vrController::setCuttingPlane(float value){
    if(Manager::param_bool[dvr::CHECK_CUTTING]) {
        if (isRayCasting()) {
            cutter_->setCutPlane(value);
            raycastRenderer_->dirtyPrecompute();
        }
        else texvrRenderer_->setCuttingPlane(value);
    }else if( Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]){
        cutter_->setCenterLinePos((int)(value * 4000.0f));
    }
}
void vrController::setCuttingPlane(int id, int delta){
    if(Manager::param_bool[dvr::CHECK_CUTTING]){
        if(isRayCasting()) {cutter_->setCuttingPlaneDelta(delta);raycastRenderer_->dirtyPrecompute();}
        else texvrRenderer_->setCuttingPlaneDelta(delta);
    }else if( Manager::param_bool[dvr::CHECK_CENTER_LINE_TRAVEL]){
        cutter_->setCenterLinePos(id, delta);
    }
}
void vrController::setCuttingPlane(glm::vec3 pp, glm::vec3 pn){
    cutter_->setCutPlane(pp, pn);
    raycastRenderer_->dirtyPrecompute();
}
float* vrController::getCuttingPlane(){
    return cutter_->getCutPlane();
}
void vrController::setCuttingParams(GLuint sp, bool includePoints){
    cutter_->setCuttingParams(sp, includePoints);
}
void vrController::SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id){
    cutter_->SwitchCuttingPlane(cut_plane_id);
    raycastRenderer_->dirtyPrecompute();
}

void vrController::setDualParameter(int id, float lv, float rv){
//    if(id == CONTRAST_LIMIT){contrast_low=lv; contrast_high=rv;baked_dirty_=true;}
}
void vrController::setRenderParam(int id, float value){
    render_params_[id] = value;Manager::baked_dirty_ = true;
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

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
    tex_masks = std::vector<Texture*>(dvr::ORGAN_END, nullptr);
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
void vrController::setupSimpleMaskTexture(int ph, int pw, int pd, GLubyte * data){
    if(tex_mask!= nullptr){delete tex_mask; tex_mask= nullptr;}
    auto vsize = ph*pw*pd;
    GLubyte * edata = new GLubyte[4*vsize];
    for(auto i=0;i<vsize;i++){
        edata[4*i] = data[i];
        edata[4*i+1] = data[i];
        edata[4*i+2] = data[i];
        edata[4*i+3] = data[i];
    }
    tex_mask = new Texture(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, pw, ph, pd, edata);
    delete[]data;
    delete[]edata;
}
void vrController::assemble_mask_texture(GLubyte* data, 
                                        int ph, int pw, int pd, 
                                        int skipy, int skipx, int skipz,
                                        int offy, int offx, int offz,
                                        int nh, int nw, int nd,
                                        dvr::ORGAN_IDS mask_id){
    glm::vec3 skip_num = glm::vec3(skipx, skipy, skipz);
    glm::vec3 shrink_factor = glm::vec3(1.0f/skipx, 1.0f/skipy, 1.0f/skipz);
    glm::vec3 skip_size = skip_num*4.0f;
    organMeshRenderer* mrenderer_;
    if(mask_id >=dvr::ORGAN_END){
        meshRenderer_ = new organMeshRenderer;
        mrenderer_ = meshRenderer_;
    }
    else{
        mesh_renders[mask_id] = new organMeshRenderer;
        mrenderer_ = mesh_renders[mask_id];
    }
    int h,w,d;
    if((offx|offy|offz) == 0){
        h = int(ph * shrink_factor.y); w = int(pw * shrink_factor.x); d = int(pd * shrink_factor.z);
    }else{
        h = int(nh * shrink_factor.y); w = int(nw * shrink_factor.x); d = int(nd * shrink_factor.z);
        mrenderer_->SetOffsetScale(ph,pw,pd,nh,nw,nd,offy,offx,offz);
    }
//    std::cout<<"size: "<<h<<" "<<w<<" "<<d<<std::endl;
    mrenderer_->Setup(h,w,d,mask_id);

    GLubyte* mask = new GLubyte[h*w*d];
    GLubyte* mbuff = mask;
    GLubyte* obuff = data + ph*pw*offz*4;
    int ori_size = ph*pw*skip_size.z, n_size = w * h;

    for(int di = 0; di < d; di++){
        int idx = 0;
        for(int yi = 0, idx_o = offy*pw*4; yi < h; yi++, idx_o+=pw*skip_size.y)
            for(int xi =0, xi_o=offx*4; xi < w; xi++,xi_o+=skip_size.x)
                mbuff[idx++] = obuff[idx_o + xi_o + 2];
            
        mbuff += n_size;
        obuff += ori_size;
    }
    if(mask_id >=dvr::ORGAN_END)setupSimpleMaskTexture(h,w,d,mask);
    else{
        if(tex_masks[mask_id]!= nullptr){delete tex_masks[mask_id]; tex_masks[mask_id]= nullptr;}
        tex_masks[mask_id] = new Texture(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, w, h, d, mask);
        delete[]mask;
    }
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

     assemble_mask_texture(data, ph, pw, pd, 4,4,4,0,0,0,0,0,0);

    // //1:bladder
    // assemble_mask_texture(data, ph, pw, pd, 4,4,2,0,0,68,512,512,86,dvr::ORGAN_BALDDER);

    // // 2:kidney
    // assemble_mask_texture(data, ph, pw, pd, 2,4,2,0,0,0,256,512,84, dvr::ORGAN_KIDNEY);
    
    // // 4:colon&all
    // assemble_mask_texture(data, ph, pw, pd, 4,4,4,0,0,0,0,0,0,dvr::ORGAN_COLON);
    // //8:spleen
    // //todo:still wrong...
    //  assemble_mask_texture(data, ph, pw, pd, 1,4,4,0,0,0,128,512,164,dvr::ORGAN_SPLEEN);
    // //  assemble_mask_texture(data, ph, pw, pd, 4,1,4,0,305,0,512,128,164,dvr::ORGAN_SPLEEN);

    // //16:ileum
    //  assemble_mask_texture(data, ph, pw, pd, 2,2,2,176,93,59,256,256,97,dvr::ORGAN_ILEUM);
    // //32 aorta
    //  assemble_mask_texture(data, ph, pw, pd, 4,2,4,0,93,15,512,256,125,dvr::ORGAN_AROTA);

}
void vrController::setupCenterLine(int id, float* data){
    line_renderers_[id] = new centerLineRenderer(id, pre_draw_);
    line_renderers_[id]->updateVertices(4000, data);
    delete[]data;
}

//1-baldder, 2-kidn 4 color 8 spleen
void vrController::onViewCreated(){
    onViewCreated(pre_draw_);
}

void vrController::onViewCreated(bool pre_draw){
    pre_draw_ = pre_draw;
    texvrRenderer_ = new texvrRenderer(pre_draw);
    raycastRenderer_ = new raycastRenderer(pre_draw);
}

void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    screenQuad::instance()->onScreenSizeChange(width, height);
}
void vrController::onDraw() {
    if(!tex_volume) return;

    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(Manager::param_bool[dvr::CHECK_DRAW_POLYGON]){
        meshRenderer_->Draw();
        // for(auto mrenderer_:mesh_renders)
        //     if(mrenderer_!=nullptr) mrenderer_->Draw();
    }

    // if(Manager::param_bool[dvr::CHECK_DRAW_VOLUME]){
    //     precompute();
    //     if(isRayCasting())  raycastRenderer_->Draw();
    //     else texvrRenderer_->Draw();
    // }
    // //draw centerline
    // for(auto line:line_renderers_)
    //     line.second->onDraw(ModelMat_ * raycastRenderer_->getDimScaleMat());
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
    // if(pre_draw_)
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
    Manager::baked_dirty_ = false;
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
    if(isRayCasting()) raycastRenderer_->setCuttingPlane(value);
    else texvrRenderer_->setCuttingPlane(value);
}
void vrController::setCuttingPlane(glm::vec3 pp, glm::vec3 pn){
    if(isRayCasting()) raycastRenderer_->setCuttingPlane(pp, pn);
}
float* vrController::getCuttingPlane(){
    return raycastRenderer_->getCuttingPlane();
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


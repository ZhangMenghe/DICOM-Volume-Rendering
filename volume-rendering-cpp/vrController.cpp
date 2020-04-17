#include "vrController.h"
#include "overlayController.h"
#include <Utils/mathUtils.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;
std::vector<bool> vrController::param_bool;
std::vector<std::string> vrController::shader_contents;
bool vrController::baked_dirty_;
int vrController::color_scheme_id;

vrController* vrController::instance(){
    return myPtr_;
}
vrController::~vrController(){
    if(camera) delete camera;
    if(texvrRenderer_) delete texvrRenderer_;
    if(raycastRenderer_) delete raycastRenderer_;
    if(bakeShader_) delete bakeShader_;
    if(tex_volume) delete tex_volume;
    if(tex_baked) delete tex_baked;
    rStates_.clear();
    param_bool.clear();
    shader_contents.clear();
}
vrController::vrController(){
    onReset();
    shader_contents = std::vector<std::string>(dvr::SHADER_END);
    myPtr_ = this;
}
void vrController::onReset() {
    if(camera){delete camera; camera= nullptr;}
    baked_dirty_ = true;
    Mouse_old = glm::fvec2(.0);
    rStates_.clear();
    cst_name="";
    _screen_w = 0; _screen_h = 0;
//    contrast_low=.0f;contrast_high=255.0f;

    setMVPStatus("default_status");
}
void vrController::assembleTexture(int w, int h, int d, float vol_thickness, GLubyte * data, int channel_num){
    texvrRenderer_->setDimension(d, vol_thickness);
    raycastRenderer_->setDimension(d, vol_thickness);
    auto vsize= w*h*d;
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
    tex_volume = new Texture(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, w, h, d, vol_data);

    auto* tb_data = new GLubyte[vsize * 4];
    if(tex_baked!= nullptr){delete tex_baked; tex_baked= nullptr;}
    tex_baked = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, w, h, d, tb_data);
    delete[]tb_data;
    delete[]vol_data;
    baked_dirty_ = true;
}
//1-baldder, 2-kidn 4 color 8 spleen
void vrController::onViewCreated(){
    texvrRenderer_ = new texvrRenderer;
    raycastRenderer_ = new raycastRenderer;
}
void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    camera->setProjMat(width, height);

    glClear(GL_COLOR_BUFFER_BIT);
    screenQuad::instance()->onScreenSizeChange(width, height);
    _screen_w = width; _screen_h = height;
}
void vrController::onDraw() {
    if(!tex_volume) return;

    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    precompute();

    if(isRayCasting())  raycastRenderer_->Draw();
    else texvrRenderer_->Draw();
}
void vrController::onTouchMove(float x, float y) {
    if(!tex_volume) return;
    if(!param_bool[dvr::CHECK_CUTTING]&&param_bool[dvr::CHECK_FREEZE_VOLUME]) return;

    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();

    float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
    Mouse_old = glm::fvec2(x, y);
    xoffset *= MOUSE_ROTATE_SENSITIVITY;
    yoffset *= -MOUSE_ROTATE_SENSITIVITY;

    if(param_bool[dvr::CHECK_FREEZE_VOLUME]){
        cuttingController::instance()->onRotate(xoffset, yoffset);
        return;
    }
//    cuttingController::instance()->setTarget(param_bool[dvr::CHECK_FREEZE_CPLANE]?VOLUME:PLANE);

    RotateMat_ = mouseRotateMat(RotateMat_, xoffset, yoffset);
    volume_model_dirty = true;
}
void vrController::onScale(float sx, float sy){
    if(!tex_volume) return;

    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
    //unified scaling
    if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
    else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;

    if(param_bool[dvr::CHECK_FREEZE_VOLUME]){
        cuttingController::instance()->onScale(sx);
    }else{
        ScaleVec3_ = ScaleVec3_* sx;
        volume_model_dirty = true;
    }
}
void vrController::onPan(float x, float y){
    if(!tex_volume|| vrController::param_bool[dvr::CHECK_FREEZE_VOLUME]) return;

    if(raycastRenderer_)isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
    float offx = x / _screen_w * MOUSE_PAN_SENSITIVITY, offy = -y /_screen_h*MOUSE_PAN_SENSITIVITY;
    PosVec3_.x += offx * ScaleVec3_.x;
    PosVec3_.y += offy * ScaleVec3_.y;
    volume_model_dirty = true;
}
void vrController::precompute(){
    if(!baked_dirty_) return;
    if(!bakeShader_){
        //geometry program
        bakeShader_ = new Shader;
        if(!bakeShader_->AddShader(GL_COMPUTE_SHADER, shader_contents[dvr::SHADER_RAYCASTVOLUME_GLSL])
           ||!bakeShader_->CompileAndLink())
            LOGE("Raycast=====Failed to create geometry shader");
        shader_contents[dvr::SHADER_RAYCASTVOLUME_GLSL]="";
    }
    overlayController::instance()->updateUniforms();
    bakeShader_->DisableAllKeyword();
    bakeShader_->EnableKeyword(COLOR_SCHEMES[color_scheme_id]);
    //todo!!!! add flip stuff
//    if(tex_volume->Depth() == 144)
    bakeShader_->EnableKeyword("FLIPY");
    bakeShader_->EnableKeyword("CONTRAST_ABSOLUTE");
    if(param_bool[dvr::CHECK_MASKON]) bakeShader_->EnableKeyword("SHOW_ORGANS");
    else bakeShader_->DisableKeyword("SHOW_ORGANS");

    GLuint sp = bakeShader_->Use();
    glBindImageTexture(0, tex_volume->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(1, tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_tex_size", glm::vec3(float(tex_volume->Width()), float(tex_volume->Height()), float(tex_volume->Depth())));
    Shader::Uniform(sp, "u_maskbits", mask_bits_);
    Shader::Uniform(sp, "u_organ_num", mask_num_);
    float* widget_data_pointer;
    int widget_num;
    overlayController::instance()->getWidgetFlatPoints(widget_data_pointer, widget_num);
    Shader::Uniform(sp, "u_opacity", 6*widget_num, widget_data_pointer);
    Shader::Uniform(sp, "u_widget_num", widget_num);

    Shader::Uniform(sp, "u_contrast_low", render_params_[RENDER_CONTRAST_LOW]);
    Shader::Uniform(sp, "u_contrast_high", render_params_[RENDER_CONTRAST_HIGH]);
    Shader::Uniform(sp, "u_contrast_level", render_params_[RENDER_CONTRAST_LEVEL]);
    Shader::Uniform(sp, "u_brightness", render_params_[RENDER_BRIGHTNESS]);

    glDispatchCompute((GLuint)(tex_volume->Width() + 7) / 8, (GLuint)(tex_volume->Height() + 7) / 8, (GLuint)(tex_volume->Depth() + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);//GL_RGBA16UI);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    bakeShader_->UnUse();
    baked_dirty_ = false;
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}

void vrController::setShaderContents(dvr::SHADER_FILES fid, std::string content){
    if(fid < dvr::SHADER_END)
        shader_contents[fid] = content;
}
void vrController::updateVolumeModelMat(){
    ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
                 * RotateMat_
                 * glm::scale(glm::mat4(1.0), ScaleVec3_);
}
void vrController::setMVPStatus(std::string status_name){
    //save changes to current status
    if(status_name == cst_name) return;
    if(!cst_name.empty()) rStates_[cst_name] = reservedStatus(ModelMat_, RotateMat_, ScaleVec3_, PosVec3_, camera);

    //restore / create status
    auto it = rStates_.find(status_name);
    if (it == rStates_.end()) {
        rStates_[status_name] = reservedStatus();
        //LOGE("===create status for %s\n", status_name.c_str());
        // for debug camera only
        //if(status_name == "Raycasting") rStates_[status_name].vcam->setPosition();
        if(_screen_w != 0)rStates_[status_name].vcam->setProjMat(_screen_w, _screen_h);
    }

    auto rstate_ = rStates_[status_name];
    ModelMat_=rstate_.model_mat; RotateMat_=rstate_.rot_mat; ScaleVec3_=rstate_.scale_vec; PosVec3_=rstate_.pos_vec; camera=rstate_.vcam;

    volume_model_dirty = false;
    cst_name = status_name;
//    auto cpos= camera->getCameraPosition();
//    LOGE("===current status %s, pos: %f, %f, %f, camera: %f, %f, %f", cst_name.c_str(), PosVec3_.x, PosVec3_.y, PosVec3_.z, cpos.x, cpos.y, cpos.z);
}
void vrController::setCuttingPlane(float value){
    if(isRayCasting()) raycastRenderer_->setCuttingPlane(value);
    else texvrRenderer_->setCuttingPlane(value);
}
void vrController::setDualParameter(int id, float lv, float rv){
//    if(id == CONTRAST_LIMIT){contrast_low=lv; contrast_high=rv;baked_dirty_=true;}
}
void vrController::setRenderParam(int id, float value){
    render_params_[id] = value;baked_dirty_ = true;
}


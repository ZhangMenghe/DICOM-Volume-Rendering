#include "vrController.h"
#include <Utils/mathUtils.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;
std::vector<float> vrController::param_tex, vrController::param_ray;
std::vector<bool> vrController::param_bool;
std::vector<std::string> vrController::shader_contents = std::vector<std::string>(14);
bool vrController::baked_dirty_ = true;
bool vrController::cutDirty = true;

vrController* vrController::instance(){
    if(!myPtr_) myPtr_ = new vrController;
    return myPtr_;
}
//void vrController::setMMS(dvr::ModelMatStatus mms){
//    ScaleVec3_ = mms.scaleVec;
//    RotateMat_ = mms.rotMat;
//    PosVec3_ = mms.posVec;
//    ModelMat_ =  mms.modelMat;
//}
//void vrController::getMMS(dvr::ModelMatStatus& mms){
//    mms.scaleVec = ScaleVec3_;
//    mms.rotMat = RotateMat_;
//    mms.posVec = PosVec3_;
//    mms.modelMat =ModelMat_;
//}
vrController::vrController(){
    shader_contents = std::vector<std::string>(SHADER_ANDROID_END);
//    onReset();
    myPtr_ = this;
}
vrController::~vrController(){
}

void vrController::onReset() {
//    ScaleVec3_ = DEFAULT_SCALE;
//    RotateMat_ = DEFAULT_ROTATE;
//    PosVec3_ = DEFAULT_POS;
//    updateVolumeModelMat();
    setStatus("default_status");
}
void vrController::setVolumeConfig(int width, int height, int dims){
    VOL_DIMS = glm::uvec3(width, height, dims);
}
void vrController::assembleTexture(GLubyte * data, int nc){
    texvrRenderer_->setDimension(VOL_DIMS.z);
    raycastRenderer_->setDimension(VOL_DIMS.z);
    auto vsize= VOL_DIMS.x * VOL_DIMS.y * VOL_DIMS.z;
    vol_data = new uint32_t[vsize];
    uint16_t tm;
    //fuse volume data
    for(auto i=0, shift = 0; i<vsize; i++, shift+=nc) {
        vol_data[i] = uint32_t((((uint32_t)data[shift+1])<<8) + (uint32_t)data[shift]);
        tm = (nc==4)?uint16_t((((uint16_t)data[shift+3])<<8)+data[shift+2]):(uint16_t)0;
        vol_data[i] = uint32_t((((uint32_t)tm)<<16)+vol_data[i]);
        // vol_data[i] = (((uint32_t)data[4*i+3])<<24)+(((uint32_t)data[4*i+3])<<16)+(((uint32_t)data[4*i+1])<<8) + ((uint32_t)data[4*i]);
    }
    tex_volume = new Texture(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, VOL_DIMS.x, VOL_DIMS.y, VOL_DIMS.z, vol_data);

    auto* tb_data = new GLubyte[vsize * 4];
    tex_baked = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, VOL_DIMS.x, VOL_DIMS.y, VOL_DIMS.z, tb_data);
    delete[]tb_data;
}

void vrController::updateTexture(GLubyte* data){
    if(!vol_data) return;
    auto vsize= VOL_DIMS.x * VOL_DIMS.y * VOL_DIMS.z;
    uint16_t tm;
    //fuse volume data
    for(auto i=0; i<vsize; i++) {
        vol_data[i] = uint32_t((((uint32_t)data[4*i+1])<<8) + (uint32_t)data[4*i]);
        tm = uint16_t((((uint16_t)data[4*i+3])<<8)+data[4*i+2]);
        vol_data[i] = uint32_t((((uint32_t)tm)<<16)+vol_data[i]);
        // vol_data[i] = (((uint32_t)data[4*i+3])<<24)+(((uint32_t)data[4*i+3])<<16)+(((uint32_t)data[4*i+1])<<8) + ((uint32_t)data[4*i]);
    }

    tex_volume->Update(vol_data);
    baked_dirty_ = true;
}
void vrController::onViewCreated(){
    texvrRenderer_ = new texvrRenderer;
    raycastRenderer_ = new raycastRenderer;
  
    funcRenderer_ = new FuncRenderer;
    funcRenderer_->CreateFunction(COLOR_BAR);
    funcRenderer_->CreateFunction(OPACITY_FUN);
}
void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    camera->setProjMat(width, height);
    _screen_w = width; _screen_h = height;
    glClear(GL_COLOR_BUFFER_BIT);
    screenQuad::instance()->onScreenSizeChange(width, height);
}
void vrController::onViewChange(int rot, int width, int height){
    onViewChange(width, height);
}
void vrController::onDraw() {
    if(!tex_volume) return;

    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(cutDirty){ //panel switch to cutting, update cutting result
        cutDirty = false;
        texvrRenderer_->onCuttingChange(param_tex[dvr::TT_CUTTING_TEX]);
        raycastRenderer_->onCuttingChange(param_ray[dvr::TR_CUTTING_RAY]);
    }
    precompute();

    if(isRayCasting()){
        raycastRenderer_->dirtyPrecompute();
        raycastRenderer_->Draw();
    }else {
        texvrRenderer_->dirtyPrecompute();
        texvrRenderer_->Draw();
    }
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
    }

    if(param_bool[dvr::CHECK_COLOR_TRANS]) bakeShader_->EnableKeyword("TRANSFER_COLOR");
    else bakeShader_->DisableKeyword("TRANSFER_COLOR");

    if(param_bool[dvr::CHECK_MASKON]) bakeShader_->EnableKeyword("SHOW_ORGANS");
    else bakeShader_->DisableKeyword("SHOW_ORGANS");

    GLuint sp = bakeShader_->Use();
    glBindImageTexture(0, tex_volume->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(1, tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    Shader::Uniform(sp, "u_maskbits", vrController::mask_bits_);
    Shader::Uniform(sp, "u_organ_num", vrController::mask_num_);

    if(isRayCasting())
        raycastRenderer_->updatePrecomputation(sp);
    else
        texvrRenderer_->updatePrecomputation(sp);

    glDispatchCompute((GLuint)(tex_volume->Width() + 7) / 8, (GLuint)(tex_volume->Height() + 7) / 8, (GLuint)(tex_volume->Depth() + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);//GL_RGBA16UI);//GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    bakeShader_->UnUse();
    baked_dirty_ = false;
    isRayCasting()?raycastRenderer_->dirtyPrecompute():texvrRenderer_->dirtyPrecompute();
}
void vrController::onDestroy(){
    //todo: do sth
}
void vrController::onPause(){
//    ar_controller_->onPause();
}
void vrController::onResume(void* env, void* context, void* activity){
//    ar_controller_->onResume(env, context, activity);
}
void vrController::setShaderContents(dvr::SHADER_FILES fid, std::string content){
    if(fid < dvr::SHADER_END)
        shader_contents[fid] = content;
}
void vrController::onDrawOverlays(){
    funcRenderer_->Draw();

void vrController::updateVolumeModelMat(){
    ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
                 * RotateMat_
                 * glm::scale(glm::mat4(1.0), ScaleVec3_);
}
  
void vrController::setStatus(std::string status_name){
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
        if(!cst_name.empty())rStates_[status_name].vcam->setProjMat(_screen_w, _screen_h);
    }

    auto rstate_ = rStates_[status_name];
    ModelMat_=rstate_.model_mat; RotateMat_=rstate_.rot_mat; ScaleVec3_=rstate_.scale_vec; PosVec3_=rstate_.pos_vec; camera=rstate_.vcam;

    volume_model_dirty = false;
    cst_name = status_name;
//    auto cpos= camera->getCameraPosition();
//    LOGE("===current status %s, pos: %f, %f, %f, camera: %f, %f, %f", cst_name.c_str(), PosVec3_.x, PosVec3_.y, PosVec3_.z, cpos.x, cpos.y, cpos.z);
}

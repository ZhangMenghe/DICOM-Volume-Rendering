#include "vrController.h"
#include <Utils/mathUtils.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;

std::vector<float> vrController::param_tex, vrController::param_ray;
std::vector<bool> vrController::param_bool;
std::vector<std::string> vrController::shader_contents = std::vector<std::string>(14);

glm::mat4 vrController::ModelMat_ = glm::mat4(1.0f);
glm::mat4 vrController::RotateMat_ = glm::mat4(1.0f);
glm::vec3 vrController::ScaleVec3_ = glm::vec3(1.0f), vrController::PosVec3_=glm::vec3(.0f);
bool vrController::baked_dirty_ = true;

bool vrController::cutDirty = true;

vrController* vrController::instance(){
    return myPtr_;
}
vrController::~vrController(){
}

vrController::vrController(){
    camera = new Camera;
    myPtr_ = this;
    onReset();
}
void vrController::onReset() {
    ScaleVec3_ = DEFAULT_SCALE;
    RotateMat_ = DEFAULT_ROTATE;
    PosVec3_ = DEFAULT_POS;
    updateVolumeModelMat();
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
void vrController::onDraw() {
    if(!tex_volume) return;

    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(cutDirty){ //panel switch to cutting, update cutting result
        cutDirty = false;
        texvrRenderer_->onCuttingChange(param_tex[dvr::TT_CUTTING_TEX]);
        raycastRenderer_->onCuttingChange(param_ray[dvr::TR_CUTTING_RAY]);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    precompute();

    if(isRayCasting())  raycastRenderer_->Draw();
    else texvrRenderer_->Draw();
    funcRenderer_->Draw();
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
void vrController::setShaderContents(dvr::SHADER_FILES fid, std::string content){
    if(fid < dvr::SHADER_END)
        shader_contents[fid] = content;
}


#include <AndroidUtils/AndroidHelper.h>
#include "vrController.h"
#include "dicomRenderer/Color.h"
#include <AndroidUtils/mathUtils.h>
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;
Texture * vrController::tex_volume= nullptr; Texture* vrController::tex_baked = nullptr; Texture* vrController::ray_baked = nullptr;
int vrController::VOLUME_TEX_ID=0, vrController::BAKED_TEX_ID = 1, vrController::BAKED_RAY_ID = 2;//, vrController::TRANS_TEX_ID = 1;
float vrController::_screen_w= .0f; float vrController::_screen_h= .0f;
std::unordered_map<std::string, float> vrController::param_value_map;
std::unordered_map<std::string, bool > vrController::param_bool_map;
glm::mat4 vrController::ModelMat_ = glm::mat4(1.0);
glm::mat4 vrController::RotateMat_ = glm::mat4(1.0f);
glm::vec3 vrController::ScaleVec3_ = glm::vec3(1.0f), vrController::PosVec3_=glm::vec3(.0f);
bool vrController::ROTATE_AROUND_CUBE = false, vrController::baked_dirty_ = true;

glm::vec3 vrController::csphere_c = glm::vec3(-1.2, -0.5, 0.5); //volume extend 0.5
float vrController::csphere_radius = 0.5f;
bool vrController::cutDirty = true;

vrController* vrController::instance(){
    return myPtr_;
}
vrController::vrController(AAssetManager *assetManager):
        _asset_manager(assetManager){
    new assetLoader(assetManager);
    camera = new Camera;
    myPtr_ = this;
    ScaleVec3_ = DEFAULT_SCALE;
    updateVolumeModelMat();
}

void vrController::assembleTexture(GLubyte * data, int width, int height, int depth, const int channel_num){
    switch(channel_num){
        case 1:
            tex_volume = new Texture(GL_R8, GL_RED, GL_UNSIGNED_BYTE, width, height, depth, data);
            break;
        case 2:
            tex_volume = new Texture(GL_RG8, GL_RG, GL_UNSIGNED_BYTE, width, height, depth, data);
            break;
        case 4:
            tex_volume = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, depth, data);
            break;
        default:
            tex_volume = new Texture(GL_R8, GL_RED, GL_UNSIGNED_BYTE, width, height, depth, data);
            break;
    }

    int vsize= width* height* depth;
    GLbyte * vdata = new GLbyte[vsize * 4];
    memset(vdata, 0xff, vsize * 4 * sizeof(GLbyte));
    tex_baked = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, depth, vdata);
    ray_baked = new Texture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, depth, vdata);
    delete[]vdata;
}

void vrController::onViewCreated(){
    texvrRenderer_ = new texvrRenderer;
    raycastRenderer_ = new raycastRenderer(true);

    funcRenderer_ = new FuncRenderer;
    funcRenderer_->CreateFunction(COLOR_BAR);
    funcRenderer_->CreateFunction(OPACITY_FUN);
}
void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    camera->setProjMat(width, height);
    _screen_w = width; _screen_h = height;
    glClear(GL_COLOR_BUFFER_BIT);
}
void vrController::onDraw() {
    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(cutDirty){ //panel switch to cutting, update cutting result
        cutDirty = false;
        texvrRenderer_->onCuttingChange(param_value_map["cutting"]);
        raycastRenderer_->onCuttingChange(param_value_map["cutting"]);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    precompute();

    if(isRayCasting())  raycastRenderer_->Draw();
    else texvrRenderer_->Draw();
    funcRenderer_->Draw();
}

void vrController::onTouchMove(float x, float y) {
    if(raycastRenderer_ && isRayCasting()) raycastRenderer_->dirtyPrecompute();

    float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
    Mouse_old = glm::fvec2(x, y);
    xoffset *= MOUSE_ROTATE_SENSITIVITY;
    yoffset *= -MOUSE_ROTATE_SENSITIVITY;

    if(ROTATE_AROUND_CUBE){
        if (fabsf(xoffset / _screen_w) > fabsf(yoffset / _screen_h)) camera->rotateCamera(3, ModelMat_[3], xoffset);
        else camera->rotateCamera(2, ModelMat_[3], yoffset);
        return;
    }
    if(param_value_map["mtarget"] > .0f && !param_bool_map["pfview"]){
        cuttingController::instance()->onRotate(mTarget((int)param_value_map["mtarget"]), xoffset, yoffset);
        return;
    }

    RotateMat_ = mouseRotateMat(RotateMat_, xoffset, yoffset);
    volume_model_dirty = true;
}
void vrController::onScale(float sx, float sy){
    if(raycastRenderer_ && isRayCasting()) raycastRenderer_->dirtyPrecompute();
    //unified scaling
    if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
    else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;

    //rotate cutting
    if(param_value_map["mtarget"] > .0f){
        mTarget tar = mTarget((int)param_value_map["mtarget"]);
        cuttingController::instance()->onScale(tar, sx);
    }else{
        ScaleVec3_ = ScaleVec3_* sx;
        volume_model_dirty = true;
    }
}
void vrController::onPan(float x, float y){
    if(raycastRenderer_ && isRayCasting()) raycastRenderer_->dirtyPrecompute();
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
        if(!bakeShader_->AddShaderFile(GL_COMPUTE_SHADER, "shaders/raycastVolume.glsl")
           ||!bakeShader_->CompileAndLink())
            LOGE("Raycast=====Failed to create geometry shader");
    }

    if(isRayCasting()) bakeShader_->EnableKeyword("UPDATE_RAY_BAKED");
    else bakeShader_->DisableKeyword("UPDATE_RAY_BAKED");

    if(param_bool_map["colortrans"]) bakeShader_->EnableKeyword("TRANSFER_COLOR");
    else bakeShader_->DisableKeyword("TRANSFER_COLOR");

    if(param_bool_map["maskon"]) bakeShader_->EnableKeyword("ORGANS_ONLY");
    else bakeShader_->DisableKeyword("ORGANS_ONLY");

    GLuint sp = bakeShader_->Use();
    glBindImageTexture(0, tex_volume->GLTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, tex_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glBindImageTexture(2, ray_baked->GLTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    if(isRayCasting())
        raycastRenderer_->updatePrecomputation(sp);
    else
        texvrRenderer_->updatePrecomputation(sp);

    glDispatchCompute((GLuint)(tex_volume->Width() + 7) / 8, (GLuint)(tex_volume->Height() + 7) / 8, (GLuint)(tex_volume->Depth() + 7) / 8);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glBindImageTexture(2, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    bakeShader_->UnUse();
    baked_dirty_ = false;
    if(isRayCasting())
        raycastRenderer_->dirtyPrecompute();
}
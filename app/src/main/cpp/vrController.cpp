#include <AndroidUtils/AndroidHelper.h>
#include "vrController.h"
#include "dicomRenderer/Color.h"
#include <AndroidUtils/mathUtils.h>
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;
Texture * vrController::tex_volume= nullptr;Texture * vrController::tex_trans= nullptr;
int vrController::VOLUME_TEX_ID=0;//, vrController::TRANS_TEX_ID = 1;
float vrController::_screen_w= .0f; float vrController::_screen_h= .0f;
std::unordered_map<std::string, float> vrController::param_value_map;
std::unordered_map<std::string, bool > vrController::param_bool_map;
glm::mat4 vrController::ModelMat_ = glm::mat4(1.0);
glm::mat4 vrController::RotateMat_ = glm::mat4(1.0f);
glm::vec3 vrController::ScaleVec3_ = glm::vec3(1.0f), vrController::PosVec3_=glm::vec3(.0f);
bool vrController::ROTATE_AROUND_CUBE = false;

glm::vec3 vrController::csphere_c = glm::vec3(-1.2, -0.5, 0.5); //volume extend 0.5
float vrController::csphere_radius = 0.5f;
bool vrController::cutDirty = true;
bool vrController::view_dirDirty = true;

vrController* vrController::instance(){
    return myPtr_;
}
vrController::vrController(AAssetManager *assetManager):
        _asset_manager(assetManager){
    new assetLoader(assetManager);
    camera = new Camera;
    myPtr_ = this;
    ScaleVec3_ = DEFAULT_SCALE;
}

void vrController::assembleTexture(GLubyte * data, int width, int height, int depth){
    tex_volume = new Texture(GL_R8, GL_RED, GL_UNSIGNED_BYTE, width, height, depth,data);
}
void vrController::setTransferColor(const int*colors, int num){
    if(num <= 0){
        colors = default_trans_color; num=default_transcolor_num;
    }
    float * transfer_color = new float[4 * num];
    getRGBAColors(colors, transfer_color, num);
    tex_trans = new Texture(GL_RGBA, GL_RGBA, GL_FLOAT, num, 1, transfer_color);
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
}
void vrController::onDraw() {
    if(volume_model_dirty){updateVolumeModelMat();volume_model_dirty = false;}
    if(cutDirty){ //panel switch to cutting, update cutting result
        cutDirty = false;
        texvrRenderer_->onCuttingChange(param_value_map["cutting"]);
        raycastRenderer_->onCuttingChange(param_value_map["cutting"]);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(param_bool_map["raycast"])
        raycastRenderer_->Draw();
    else
        texvrRenderer_->Draw();
    funcRenderer_->Draw();
}

void vrController::onTouchMove(float x, float y) {
    float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
    Mouse_old = glm::fvec2(x, y);
    xoffset *= MOUSE_ROTATE_SENSITIVITY;
    yoffset *= -MOUSE_ROTATE_SENSITIVITY;

    if(param_value_map["mtarget"] > .0f ){//rotate cutting plane when plane is rotable
        mTarget tar = mTarget((int)param_value_map["mtarget"]);
        if(param_bool_map["pfview"]){
            //todo:rotate volume only
            RotateMat_ = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(0,1,0))
                         * glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(1,0,0))
                         * RotateMat_;
            volume_model_dirty = true;
//            cuttingController::instance()->onRotate(tar);
        }else// rotate the plane only
            cuttingController::instance()->onRotate(tar, xoffset, yoffset);
        return;
    }
    if(ROTATE_AROUND_CUBE){
        if (fabsf(xoffset / _screen_w) > fabsf(yoffset / _screen_h)) camera->rotateCamera(3, ModelMat_[3], xoffset);
        else camera->rotateCamera(2, ModelMat_[3], yoffset);
    }else{
        //Rotate volume and plane together
        RotateMat_ = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(0,1,0))
                     * glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(1,0,0))
                     * RotateMat_;
        volume_model_dirty = true;

    }
}
void vrController::onScale(float sx, float sy){
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


//    if(sx == sy){
//        if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
//        else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;
//        ScaleVec3_ = ScaleVec3_* sx;
//
//        return;
//    }
//    //+ (sy- last_scale.y)*0.05f
//    if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
//    else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;
//
//    if(sy > 1.0f) sy = 1.0f + (sy - 1.0f) * MOUSE_SCALE_SENSITIVITY;
//    else sy = 1.0f - (1.0f - sy)* MOUSE_SCALE_SENSITIVITY;
//
//
//    //todo:change it via view dir
////    glm::vec3 view_dir = camera->getViewDirection();
//    float x = 1.0, y=1.0, z=1.0;
//    x = sx;y=sy;
////    float a = glm::dot(camera->getRightDir(), glm::vec3(1.0f, .0f, .0f));
////    float b = glm::dot(camera->getRightDir(), glm::vec3(.0f, 1.0f, .0f));
////    float c = glm::dot(camera->getRightDir(), glm::vec3(.0f, .0f, 1.0f));
////    if(a >=b && a>=c)
////    {x = sx;y=sy;}
////    else if(b >= a && b>=c)
////    {y = sx;x = sy;}
////    else
////    {z = sx;y=sy;}
//    ScaleVec3_ = glm::vec3(last_scale.x*x, last_scale.y*y, last_scale.z*z);
////    ModelMat_ = glm::scale(glm::mat4(1.0), ScaleVec3_);
//    last_scale = ScaleVec3_;
}
void vrController::onPan(float x, float y){
    float offx = x / _screen_w, offy = -y /_screen_h;
    PosVec3_.x += offx * ScaleVec3_.x;
    PosVec3_.y += offy * ScaleVec3_.y;
    volume_model_dirty = true;
}
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
glm::mat4 vrController::ModelMat_ = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 0.5f));
glm::mat4 vrController::RotateMat_ = glm::mat4(1.0f);
glm::vec3 vrController::ScaleVec3_ = glm::vec3(1.0f), vrController::PosVec3_=glm::vec3(.0f);

glm::vec3 vrController::csphere_c = glm::vec3(-0.5, 0.5, 0.5); //volume extend 0.5
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
    ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
               * RotateMat_
               * glm::scale(glm::mat4(1.0), ScaleVec3_);

    if(cutDirty){
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
void vrController::onScale(float sx, float sy){
    if(sx == sy){
        if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
        else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;

        ScaleVec3_ = ScaleVec3_* sx; return;
    }
    //+ (sy- last_scale.y)*0.05f
    if(sx > 1.0f) sx = 1.0f + (sx - 1.0f) * MOUSE_SCALE_SENSITIVITY;
    else sx = 1.0f - (1.0f - sx)* MOUSE_SCALE_SENSITIVITY;

    if(sy > 1.0f) sy = 1.0f + (sy - 1.0f) * MOUSE_SCALE_SENSITIVITY;
    else sy = 1.0f - (1.0f - sy)* MOUSE_SCALE_SENSITIVITY;


    //todo:change it via view dir
//    glm::vec3 view_dir = camera->getViewDirection();
    float x = 1.0, y=1.0, z=1.0;
    x = sx;y=sy;
//    float a = glm::dot(camera->getRightDir(), glm::vec3(1.0f, .0f, .0f));
//    float b = glm::dot(camera->getRightDir(), glm::vec3(.0f, 1.0f, .0f));
//    float c = glm::dot(camera->getRightDir(), glm::vec3(.0f, .0f, 1.0f));
//    if(a >=b && a>=c)
//    {x = sx;y=sy;}
//    else if(b >= a && b>=c)
//    {y = sx;x = sy;}
//    else
//    {z = sx;y=sy;}
    ScaleVec3_ = glm::vec3(last_scale.x*x, last_scale.y*y, last_scale.z*z);
//    ModelMat_ = glm::scale(glm::mat4(1.0), ScaleVec3_);
    last_scale = ScaleVec3_;
}
void vrController::onPan(float x, float y){
    float offx = x / _screen_w, offy = y /_screen_h;
    PosVec3_.x += offx * ScaleVec3_.x;// * 0.1f;
    PosVec3_.y += offy * ScaleVec3_.y;// * 0.1f;

//    getScreenToClientPos(x, y, _screen_w, _screen_h);
//    //get 3d from 2d
//    glm::mat4 inv_mat = glm::inverse(camera->getProjMat() * camera->getViewMat());
//    glm::vec4 near_plane_pos = inv_mat * glm::vec4(x, y ,-1.0f, 1.0f);
//    float inv_w = 1.0f / near_plane_pos.w;
//    PosVec3_.x = near_plane_pos.x * inv_w; PosVec3_.y = near_plane_pos.y * inv_w;
}
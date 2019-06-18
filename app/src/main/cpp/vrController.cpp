#include <AndroidUtils/AndroidHelper.h>
#include "vrController.h"
#include "dicomRenderer/Color.h"
vrController* vrController::myPtr_ = nullptr;
Camera* vrController::camera = nullptr;
Texture * vrController::tex_volume= nullptr;Texture * vrController::tex_trans= nullptr;
int vrController::VOLUME_TEX_ID=0, vrController::TRANS_TEX_ID = 1;
bool vrController::b_use_color_transfer = false;
float vrController::opa_oa = 1.0f; float vrController::opa_ll = 1.0f; float vrController::opa_co=.0f;
float vrController::_screen_w= .0f; float vrController::_screen_h= .0f;
////std::unordered_map<std::string, float> vrController::param_value_map = {};
////std::unordered_map<std::string, bool > vrController::param_bool_map = {};
//int vrController::jui_status[2] = {0,0};

vrController* vrController::instance(){
    return myPtr_;
}
vrController::vrController(AAssetManager *assetManager):
        _asset_manager(assetManager){
    new assetLoader(assetManager);
    camera = new Camera;
    myPtr_ = this;
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
}
void vrController::onViewChange(int width, int height){
    glViewport(0, 0, width, height);
    camera->setProjMat(width, height);
    _screen_w = width; _screen_h = height;
    glClear(GL_COLOR_BUFFER_BIT);
}
void vrController::onDraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    texvrRenderer_->Draw();
}
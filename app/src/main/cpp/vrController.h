#ifndef VR_CONTROLLER_H
#define VR_CONTROLLER_H

#include <android/asset_manager.h>
#include <dicomRenderer/texturebasedRenderer.h>
#include <dicomRenderer/raycastRenderer.h>
#include <GLES3/gl32.h>
#include <GLPipeline/Texture.h>
#include "GLPipeline/Camera.h"
#include "nEntrance.h"
#include "dicomRenderer/funcsRenderer.h"
#include <unordered_map>
#include <vector>

class vrController:public nEntrance{
public:
    static Texture *tex_volume, *tex_trans;
    static Camera* camera;
    static int VOLUME_TEX_ID;//, TRANS_TEX_ID;
    static float _screen_w, _screen_h;
    static std::unordered_map<std::string, float> param_value_map;
    static std::unordered_map<std::string, bool > param_bool_map;
    static glm::mat4 ModelMat_;

    static vrController* instance();

    vrController(AAssetManager *assetManager);
    void assembleTexture(GLubyte * data, int width, int height, int depth);
    void setTransferColor(const int*colors, int num = 0);
    void onViewCreated();
    void onViewChange(int width, int height);
    void onDraw();

    void onSingleTouchDown(float x, float y){
        Mouse_old = glm::fvec2(x, y);
    }
    void onTouchMove(float x, float y) {
        if (!texvrRenderer_) return;
        //Camera::instance()->Rotate_Camera(x - Mouse_old.x, Mouse_old.y - y);
        float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
        Mouse_old = glm::fvec2(x, y);
        xoffset *= MOUSE_ROTATE_SENSITIVITY;
        yoffset *= MOUSE_ROTATE_SENSITIVITY;
        if (fabsf(xoffset / _screen_w) > fabsf(yoffset / _screen_h))
            camera->rotateCamera(3, ModelMat_[3], xoffset);
         else
             camera->rotateCamera(2, ModelMat_[3], -yoffset);

    }

private:
    static vrController* myPtr_;
    AAssetManager* _asset_manager;
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    FuncRenderer* funcRenderer_ = nullptr;

    glm::fvec2 Mouse_old = glm::fvec2(.0);
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;



};
#endif
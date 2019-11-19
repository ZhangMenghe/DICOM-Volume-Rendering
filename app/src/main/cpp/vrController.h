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
    static Texture *tex_volume, *tex_baked, *ray_baked;
    static Camera* camera;
    static int VOLUME_TEX_ID, BAKED_TEX_ID, BAKED_RAY_ID;
    static float _screen_w, _screen_h;
    static std::unordered_map<std::string, float> param_value_map;
    static std::unordered_map<std::string, bool > param_bool_map;
    static glm::mat4 ModelMat_, RotateMat_;
    static glm::vec3 ScaleVec3_, PosVec3_;
    static bool ROTATE_AROUND_CUBE;
    static bool baked_dirty_;
    static int VOLUME_DIMS;
    inline static bool isRayCasting(){return param_bool_map["raycast"];}


    static glm::vec3 csphere_c;
    static float csphere_radius;
    static bool cutDirty;

    static vrController* instance();

    vrController(AAssetManager *assetManager);
    void assembleTexture(GLubyte * data, int width, int height, int depth, const int channel_num);
    void assembleTexture(GLubyte * data);
    void setVolumeConfig(int width, int height, int dims, const int channel_num);
    void onViewCreated();
    void onViewChange(int width, int height);
    void onDraw();

    void onSingleTouchDown(float x, float y){ Mouse_old = glm::fvec2(x, y);}
    void onTouchMove(float x, float y);
    void onScale(float sx, float sy);
    void onPan(float x, float y);
private:
    static vrController* myPtr_;
    AAssetManager* _asset_manager;
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    FuncRenderer* funcRenderer_ = nullptr;

    glm::fvec2 Mouse_old = glm::fvec2(.0);
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float MOUSE_SCALE_SENSITIVITY = 0.8f;
    const float MOUSE_PAN_SENSITIVITY = 1.2f;
    const glm::vec3 DEFAULT_SCALE = glm::vec3(1.0f, 1.0f,0.5f);

    bool volume_model_dirty = true;


    Shader* bakeShader_ = nullptr;

    void updateVolumeModelMat(){
        ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
                     * RotateMat_
                     * glm::scale(glm::mat4(1.0), ScaleVec3_);
    }
    void precompute();

};
#endif
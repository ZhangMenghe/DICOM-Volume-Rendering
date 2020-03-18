#ifndef VR_CONTROLLER_H
#define VR_CONTROLLER_H

#include <dicomRenderer/texturebasedRenderer.h>
#include <dicomRenderer/raycastRenderer.h>
#include <GLPipeline/Texture.h>
#include "GLPipeline/Camera.h"
#include "nEntrance.h"
#include "dicomRenderer/funcsRenderer.h"
#include "dicomRenderer/Constants.h"
#include <unordered_map>
#include <vector>

#ifdef __ANDROID
    #include <Jnis/jni_main.h>
#endif
class AAssetManager;

class vrController:public nEntrance{
public:
    static Texture *tex_volume, *tex_baked, *ray_baked;
    static Camera* camera;
    static int VOLUME_TEX_ID, BAKED_TEX_ID, BAKED_RAY_ID;
    static float _screen_w, _screen_h;

    static std::vector<float> param_tex, param_ray;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents;
    static glm::mat4 ModelMat_, RotateMat_;
    static glm::vec3 ScaleVec3_, PosVec3_;
    static glm::uvec3 VOL_DIMS;
    static bool ROTATE_AROUND_CUBE;
    static bool baked_dirty_;

    static unsigned int mask_num_, mask_bits_;

    inline static bool isRayCasting(){
        return param_bool[dvr::CHECK_RAYCAST];
    }

    static glm::vec3 csphere_c;
    static float csphere_radius;
    static bool cutDirty;

    static vrController* instance();

    vrController(AAssetManager *assetManager = nullptr);
    ~vrController();
    void assembleTexture(GLubyte * data, int channel_num = 4);
    void updateTexture(GLubyte * data);
    void setVolumeConfig(int width, int height, int dims);

    /*Override*/
    void onViewCreated();
    void onViewChange(int width, int height);
    void onViewChange(int rot, int width, int height);
    void onDraw();
    void onReset();
    void onDestroy();

    void onSingleTouchDown(float x, float y){ Mouse_old = glm::fvec2(x, y);}
    void onTouchMove(float x, float y);
    void onScale(float sx, float sy);
    void onPan(float x, float y);

    void setShaderContents(dvr::SHADER_FILES fid, std::string content);
private:
    static vrController* myPtr_;
    AAssetManager* _asset_manager;
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    FuncRenderer* funcRenderer_ = nullptr;

    glm::fvec2 Mouse_old = glm::fvec2(.0);
    bool volume_model_dirty = true;

    Shader* bakeShader_ = nullptr;
    uint32_t* vol_data = nullptr;

    void updateVolumeModelMat(){
        ModelMat_ =  glm::translate(glm::mat4(1.0), PosVec3_)
                     * RotateMat_
                     * glm::scale(glm::mat4(1.0), ScaleVec3_);
    }
    void precompute();

};
#endif
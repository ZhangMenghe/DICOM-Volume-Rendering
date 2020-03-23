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

#ifdef __ANDROID__
    #include <platforms/android/Jnis/jni_main.h>
#endif
class vrController:public nEntrance{
public:
    static Camera* camera;
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

    vrController();
    ~vrController();
    void assembleTexture(GLubyte * data, int channel_num = 4);
    void updateTexture(GLubyte * data);
    void setVolumeConfig(int width, int height, int dims);
    void onDrawOverlays();
    Texture* getTex(dvr::TEX_IDS id){
        if(id == dvr::BAKED_TEX_ID) return tex_baked;
        return ray_baked;
    }

    /*Override*/
    void onViewCreated();
    void onViewChange(int width, int height);
    void onViewChange(int rot, int width, int height);
    void onDraw();
    void onReset();
    void onPause();
    void onDestroy();
    void onResume(void* env, void* context, void* activity);

    void onSingleTouchDown(float x, float y){ Mouse_old = glm::fvec2(x, y);}
    void onTouchMove(float x, float y);
    void onScale(float sx, float sy);
    void onPan(float x, float y);

    void setShaderContents(dvr::SHADER_FILES fid, std::string content);
private:
    static vrController* myPtr_;

    //renderers
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    FuncRenderer* funcRenderer_ = nullptr;

    //Textures
    Texture *tex_volume= nullptr, *tex_baked = nullptr, *ray_baked = nullptr;

    //ui
    glm::fvec2 Mouse_old = glm::fvec2(.0);

    //flags
    bool volume_model_dirty = true;

    Shader* bakeShader_ = nullptr;
    uint32_t* vol_data = nullptr;

    void updateVolumeModelMat();
    void precompute();

};
#endif
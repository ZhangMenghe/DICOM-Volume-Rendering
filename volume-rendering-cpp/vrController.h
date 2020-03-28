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
#include <map>

class vrController:public nEntrance{
public:
    static Camera* camera;
    static std::vector<float> param_tex, param_ray;
    static std::vector<bool> param_bool;
    static std::vector<std::string> shader_contents;

    static bool baked_dirty_;
    static bool cutDirty;
    unsigned int mask_num_, mask_bits_;

    static vrController* instance();

    vrController();
    ~vrController();
    void assembleTexture(GLubyte * data, int channel_num = 4);
    void updateTexture(GLubyte * data);
    void setVolumeConfig(int width, int height, int dims);
    void onDrawOverlays();

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

    //setter funcs
    void setShaderContents(dvr::SHADER_FILES fid, std::string content);
    void setStatus(std::string status_name);
    void setVolumeRST(glm::mat4 rm, glm::vec3 sv, glm::vec3 pv){RotateMat_=rm; ScaleVec3_=sv; PosVec3_=pv; volume_model_dirty=true;}
    //getter funcs
    GLuint getBakedTex(){return tex_baked->GLTexture();}
    glm::mat4 getModelMatrix(){return ModelMat_;}
    glm::mat4 getRotationMatrix(){return RotateMat_;}
private:
    static vrController* myPtr_;

    //renderers
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    FuncRenderer* funcRenderer_ = nullptr;

    //Shader
    Shader* bakeShader_ = nullptr;

    //Textures
    Texture *tex_volume = nullptr, *tex_baked = nullptr;

    //volume datas
    glm::uvec3 VOL_DIMS = glm::uvec3(0);
    uint32_t* vol_data = nullptr;

    struct reservedStatus{
        glm::mat4 model_mat, rot_mat;
        glm::vec3 scale_vec, pos_vec;
        Camera* vcam;
        reservedStatus(glm::mat4 mm, glm::mat4 rm, glm::vec3 sv, glm::vec3 pv, Camera* cam){
            model_mat=mm; rot_mat=rm; scale_vec=sv; pos_vec=pv; vcam=cam;
        }
        reservedStatus():rot_mat(dvr::DEFAULT_ROTATE), scale_vec(dvr::DEFAULT_SCALE), pos_vec(dvr::DEFAULT_POS), vcam(new Camera){
            model_mat =  glm::translate(glm::mat4(1.0), pos_vec)
                         * rot_mat
                         * glm::scale(glm::mat4(1.0), scale_vec);
        }
    };
    std::string cst_name;
    std::map<std::string, reservedStatus> rStates_;
    glm::mat4 ModelMat_, RotateMat_;
    glm::vec3 ScaleVec3_, PosVec3_;

    //ui
    glm::fvec2 Mouse_old;
    float _screen_w, _screen_h;

    //flags
    bool volume_model_dirty;

    void updateVolumeModelMat();
    void precompute();
    bool isRayCasting(){
        return param_bool[dvr::CHECK_RAYCAST];
    }
};
#endif
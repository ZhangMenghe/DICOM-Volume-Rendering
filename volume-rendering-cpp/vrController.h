#ifndef VR_CONTROLLER_H
#define VR_CONTROLLER_H

#include <dicomRenderer/texturebasedRenderer.h>
#include <dicomRenderer/raycastRenderer.h>
#include <dicomRenderer/organMeshRenderer.h>
#include <dicomRenderer/centerLineRenderer.h>
#include <dicomRenderer/cuttingController.h>
#include <Utils/perfMonitor.h>
#include <GLPipeline/Texture.h>
#include "nEntrance.h"
#include "dicomRenderer/Constants.h"
#include "Manager.h"
#include <unordered_map>
#include <map>

class vrController:public nEntrance{
public:
    unsigned int mask_num_, mask_bits_;
    static vrController* instance();
    bool isDrawing(){return tex_volume!= nullptr;}

    vrController();
    ~vrController();
    void assembleTexture(int update_target, int ph, int pw, int pd, float sh, float sw, float sd, GLubyte * data, int channel_num = 4);
    void setupCenterLine(int id, float* data);
    /*Override*/
    void onViewCreated();
    void onViewCreated(bool pre_draw);
    void onViewChange(int width, int height);
    void onDraw();
    void onReset();
    void onReset(glm::vec3 pv, glm::vec3 sv, glm::mat4 rm, Camera* cam);

    void onSingleTouchDown(float x, float y){ Mouse_old = glm::fvec2(x, y);}
    void onTouchMove(float x, float y);
    void onScale(float sx, float sy);
    void onPan(float x, float y);

    //setter funcs
    void setShaderContents(dvr::SHADER_FILES fid, std::string content);
    void setMVPStatus(std::string status_name);
    void setCuttingPlane(float value);
    void setCuttingPlane(glm::vec3 pp, glm::vec3 pn);
    void setDualParameter(int id, float lv, float rv);
    void setRenderParam(int id, float value);
    void setRenderParam(float* values){memcpy(render_params_, values, dvr::PARAM_RENDER_TUNE_END*sizeof(float));Manager::baked_dirty_=true;}

    //getter funcs
    GLuint getVolumeTex(){return tex_volume->GLTexture();}
    GLuint getBakedTex(){return tex_baked->GLTexture();}
    glm::mat4 getModelMatrix(bool dim_scaled = false){
        return dim_scaled?ModelMat_ * raycastRenderer_->getDimScaleMat():ModelMat_;}
    glm::mat4 getRotationMatrix(){return RotateMat_;}
    float* getCurrentReservedStates();
    float* getCuttingPlane();
    void setCuttingParams(GLuint sp, bool includePoints = false);
private:
    static vrController* myPtr_;

    //renderers
    texvrRenderer* texvrRenderer_ = nullptr;
    raycastRenderer* raycastRenderer_ = nullptr;
    organMeshRenderer* meshRenderer_ = nullptr;
    std::vector<organMeshRenderer*> mesh_renders;
    std::unordered_map<int, centerLineRenderer*> line_renderers_;
    cuttingController* cutter_;

    //Shader
    Shader* bakeShader_ = nullptr;

    //Textures
    Texture *tex_volume = nullptr, *tex_baked = nullptr;

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
    float render_params_[dvr::PARAM_RENDER_TUNE_END]={.0f};
    bool pre_draw_ = true;

    //ui
    glm::fvec2 Mouse_old;

    //flags
    bool volume_model_dirty;
    
    //performance 
    perfMonitor pm_;

    void updateVolumeModelMat();
    void precompute();
    bool isRayCasting(){return Manager::param_bool[dvr::CHECK_RAYCAST];}
};
#endif
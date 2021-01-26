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
    void onViewChange(int width, int height);
    void onViewChange(int rot, int width, int height){onViewChange(width, height);}
    void onDrawScene();
    void onDraw();
    void onReset();
    void onPause(){}
    void onDestroy(){}
    void onResume(void* env, void* context, void* activity){}
    void onReset(glm::vec3 pv, glm::vec3 sv, glm::mat4 rm, Camera* cam);
    void AlignModelMatToTraversalPlane();

    void onSingleTouchDown(float x, float y);
    void onTouchMove(float x, float y);
    void onScale(float sx, float sy);
    void onPan(float x, float y);

    //setter funcs
    void setPredrawOption(bool pre_draw){pre_draw_=pre_draw;}
    static void setShaderContents(dvr::SHADER_FILES fid, std::string content);
    void setMVPStatus(std::string status_name);
    void setCuttingPlane(float value);
    void setCuttingPlane(int id, int delta);
    void setCuttingPlane(glm::vec3 pp, glm::vec3 pn);
    void setCuttingParams(GLuint sp);
    void setDualParameter(int id, float lv, float rv);
    void setRenderParam(int id, float value);
    void setRenderParam(float* values);
    void setVolumeRST(glm::mat4 rm, glm::vec3 sv, glm::vec3 pv);
    void setVolumePosition(glm::vec3 pv){PosVec3_ = pv;volume_model_dirty=true;}
    bool addStatus(std::string name, glm::mat4 mm, glm::mat4 rm, glm::vec3 sv, glm::vec3 pv, Camera* cam);
    bool addStatus(std::string name, bool use_current_status = false);
    void SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id);

    //getter funcs
    GLuint getVolumeTex(){return tex_volume->GLTexture();}
    GLuint getBakedTex(){return tex_baked->GLTexture();}
    glm::mat4 getModelMatrix(bool dim_scaled = false){
        return dim_scaled?ModelMat_ * vol_dim_scale_mat_:ModelMat_;}
    glm::mat4 getRotationMatrix(){return RotateMat_;}
    glm::vec3 getModelPos(){return PosVec3_;}
    float* getCurrentReservedStates();
    float* getCuttingPlane();
    void getCuttingPlane(glm::vec3& pp, glm::vec3& pn){cutter_->getCuttingPlane(pp,pn);}
    bool isDirty();

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

    //volume
    glm::vec3 vol_dimension_, vol_dim_scale_;
    glm::mat4 vol_dim_scale_mat_;

    //ui
    glm::fvec2 Mouse_old;

    //flags
    bool volume_model_dirty;
    
    //performance 
    perfMonitor pm_;

    //vec3
    glm::vec4 ray_dir;
    bool ray_initialized = false;

    //color scheme
    std::vector<float*> m_color_rgb = std::vector<float*>(5, nullptr);
    
    static bool isRayCasting(){return Manager::param_bool[dvr::CHECK_RAYCAST];}
    void updateVolumeModelMat();
    void precompute();
    bool check_ar_ray_intersect();
};
#endif
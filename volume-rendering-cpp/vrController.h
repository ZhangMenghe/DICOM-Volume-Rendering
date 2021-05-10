#ifndef VR_CONTROLLER_H
#define VR_CONTROLLER_H

#include <dicomRenderer/baseDicomRenderer.h>

#include <dicomRenderer/organMeshRenderer.h>
#include <dicomRenderer/centerLineRenderer.h>
#include <dicomRenderer/cuttingController.h>
#include <Utils/perfMonitor.h>
#include <GLPipeline/Texture.h>
#include "nEntrance.h"
#include "dicomRenderer/Constants.h"
#include "Manager.h"
#include <map>
#include <dicomRenderer/dataVisualizer.h>

class vrController:public nEntrance{
public:
    static vrController* instance();
    bool isDrawing(){return tex_volume!= nullptr;}

    vrController(const std::shared_ptr<Manager> &manager);
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
    void setCuttingPlane(float value);
    void setCuttingPlane(int id, int delta);
    void setCuttingPlane(glm::vec3 pp, glm::vec3 pn);
    void setCuttingParams(GLuint sp);
    void setVolumeRST(glm::mat4 rm, glm::vec3 sv, glm::vec3 pv);
    void setVolumePosition(glm::vec3 pv){PosVec3_ = pv;volume_model_dirty=true;}
    void SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id);
    void setOverlayRects(int id, int width, int height, int left, int top);
    void setRenderingMethod(dvr::RENDER_METHOD method){
        if(m_rmethod_id == method) return;
        m_rmethod_id = method;//vRenderer_[m_rmethod_id]->dirtyPrecompute();
    }

    //getter funcs
    GLuint getVolumeTex(){return tex_volume->GLTexture();}
    GLuint getBakedTex(){return tex_baked->GLTexture();}
    glm::mat4 getModelMatrix(bool dim_scaled = false){
        return dim_scaled?ModelMat_ * vol_dim_scale_mat_:ModelMat_;}
    glm::vec3 getModelPos(){return PosVec3_;}
    float* getCurrentReservedStates();
    float* getCuttingPlane();
    void getCuttingPlane(glm::vec3& pp, glm::vec3& pn){cutter_->getCuttingPlane(pp,pn);}
    bool isDirty();

    bool isRayCut(){return isRayCasting() && Manager::param_bool[dvr::CHECK_CUTTING];}

private:
    static vrController* myPtr_;
    std::shared_ptr<Manager> m_manager;

    //renderers
    std::vector<baseDicomRenderer*> vRenderer_;
    int m_rmethod_id = -1;

    organMeshRenderer* meshRenderer_ = nullptr;
    std::vector<organMeshRenderer*> mesh_renders;
    std::unordered_map<int, centerLineRenderer*> line_renderers_;
    cuttingController* cutter_;
    dataBoard *data_board_;

    //Shader
    Shader* bakeShader_ = nullptr;

    //Textures
    Texture *tex_volume = nullptr, *tex_baked = nullptr;

    glm::mat4 ModelMat_, RotateMat_;
    glm::vec3 ScaleVec3_, PosVec3_;
    bool pre_draw_ = false;

    //volume
    glm::vec3 vol_dimension_, vol_dim_scale_;
    glm::mat4 vol_dim_scale_mat_;

    //ui
    glm::fvec2 Mouse_old;

    //flags
    bool volume_model_dirty, volume_rotate_dirty;
    
    //performance 
    perfMonitor pm_;

    //vec3
    glm::vec4 ray_dir;
    bool ray_initialized = false;

    const float m_inverse_[16] = {
            1.0, -1.0, -1.0, 1.0,
            1.0,-1.0,-1.0,1.0,
            1.0,-1.0,-1.0,1.0,
            1.0, -1.0, -1.0, 1.0 };
    
    bool isRayCasting(){return m_rmethod_id == (int)dvr::RAYCASTING;}
    void updateVolumeModelMat();
    void precompute();
    bool check_ar_ray_intersect();
};
#endif
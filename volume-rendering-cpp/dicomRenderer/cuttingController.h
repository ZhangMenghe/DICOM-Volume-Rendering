#ifndef DICOM_VOLUME_RENDERING_CUTTINGCONTROLLER_H
#define DICOM_VOLUME_RENDERING_CUTTINGCONTROLLER_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <GLPipeline/Shader.h>
#include <dicomRenderer/Constants.h>
typedef struct{
    glm::vec3 p;
    glm::vec3 n;
}cPlane;
typedef struct{
    glm::vec3 c;
    float r;
}cSphere;
typedef enum{
    PLANE = 1,
    SPHERE,
    VOLUME
}mTarget;

class cuttingController {
private:
    const float CUTTING_RADIUS = 0.5f;
    const float DEFAULT_CUTTING_SCALE = 1.5f;
    const float DEFAULT_TRAVERSAL_SCALE = 0.1f;

    const int center_sample_gap = 5;
    float CMOVE_UNIT_SIZE = 0.02f;

    cPlane cplane_;
    cSphere csphere_;

    Shader* pshader = nullptr;
    GLuint pVAO_ = 0;
    GLuint frame_buff_ = 0;
    bool baked_dirty = true;

    const glm::vec4 P_Points[3] = {
        glm::vec4(-0.5f,0.5f,.0f,1.0f),
                glm::vec4(0.5f,0.5f,.0f,1.0f),
                glm::vec4(-0.5f,-0.5f,.0f,1.0f)
    };
    //in object coordinate
    glm::vec3 p_start_, p_norm_, p_point_;
    glm::vec3 p_scale = glm::vec3(1.0f);
    glm::mat4 p_rotate_mat_ = glm::mat4(1.0f);

    //in world space
    glm::vec3 p_point_world;

    //cached transformation matrix
    glm::mat4 p_p2w_mat, p_p2o_mat;
    bool p_p2o_dirty = true;

    //reserved params
    struct reservedVec{
        glm::vec3 point, scale;
        glm::mat4 rotate_mat;
        float move_value;
        reservedVec(){}
    };

    //Center Line Traversal cutting position, range[0,4000]
    int clp_id_;
    reservedVec rc, rt;
    dvr::PARAM_CUT_ID last_mode = dvr::CUT_CUTTING_PLANE;
    float cmove_value = .0f;
    glm::vec4 plane_color_ = glm::vec4(1.0, .0, .0, 0.4f);
    const float CUTTING_FACTOR = 0.00002f;
    mTarget current_target = VOLUME;
    std::unordered_map<dvr::ORGAN_IDS, float*> pmap;
    float thick_scale;
    bool centerline_available;

    void draw_plane();
    void draw_baked();
    bool keep_cutting_position();
    void update_modelMat_o();
    void update_plane_(glm::mat4 rotMat);
    void update_plane_(glm::vec3 pNorm);
    void set_centerline_cutting(int& id, glm::vec3& pp, glm::vec3& pn);

public:
    static cuttingController* _mptr;
    static cuttingController* instance();
    cuttingController();
    cuttingController(glm::vec3 ps, glm::vec3 pn);
    void setTarget(mTarget target){current_target = target;}
    void Update();
    void UpdateAndDraw();
    void Draw(bool pre_draw);
    void setCuttingParams(GLuint sp);
    void SwitchCuttingPlane(dvr::PARAM_CUT_ID cut_plane_id);
    void setupCenterLine(dvr::ORGAN_IDS id, float* data);
    void setCenterLinePos(int id, int delta_id = 0);
    void setDimension(int pd, float thickness_scale){CMOVE_UNIT_SIZE = 1.0f / (float)pd;thick_scale = thickness_scale;centerline_available= false;}

    void setCutPlane(float value);
    void setCuttingPlaneDelta(int delta);
    void setCutPlane(glm::vec3 normal);
    void setCutPlane(glm::vec3 startPoint, glm::vec3 normal);
    float* getCutPlane();
    void getCurrentTraversalInfo(glm::vec3& pp, glm::vec3& pn);
    glm::mat4 getRotationMat(){return p_rotate_mat_;}
    void onReset();
    void onRotate(float offx, float offy);
    void onScale(float sx, float sy=-1.0f, float sz=-1.0f);
    void onTranslate(float offx, float offy);
    bool IsCenterLineAvailable(){return centerline_available;}
    bool isPrecomputeDirty(){return baked_dirty;}
    void dirtyPrecompute(){baked_dirty = true;}
    void getCuttingPlane(glm::vec3& pp, glm::vec3& pn);
};


#endif

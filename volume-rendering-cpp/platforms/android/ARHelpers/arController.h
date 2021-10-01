#ifndef AR_CONTROLLER_H
#define AR_CONTROLLER_H
#include <arcore_c_api.h>
#include <platforms/platform.h>
#include <nEntrance.h>
#include <glm/glm.hpp>
#include <platforms/android/Renderers/backgroundRenderer.h>
#include <platforms/android/Renderers/PointCloudRenderer.h>
#include <platforms/android/Renderers/PlaneRenderer.h>
#include <platforms/android/Renderers/lineRenderer.h>
#include <platforms/android/Renderers/cutplaneRenderer.h>
#include <opencv2/core.hpp>
#include "ArucoMarkerTracker.h"
class arController:public nEntrance{
public:
    static arController * instance();
    arController();
    ~arController();

    void onViewChange(int rot, int width, int height);
    void onPause();
    void onResume(void* env, void* context, void* activity);

    void onDrawARChanged(bool b_ar_on);
    void onTrackingMethodChanged(bool use_ar_core);

    int getTouchedAnchorNum(){return anchor_id;}
    void getTouchedAnchor(glm::mat4& rotMat, glm::vec3& pos);
    bool getTouchedPosition(glm::vec3& pos);

    /*Override*/
    void onViewCreated();
    void onViewChange(int width, int height){}
    void onDraw();
    void onReset();
    void onDestroy(){}

    void onSingleTouchDown(float x, float y);
    void onSingleTouchUp();
    void onTouchMove(float x, float y){}
    void onScale(float sx, float sy){}
    void onPan(float x, float y){}

    bool onLongPress(float x, float y);

    //plane
    struct tPlane{
        glm::mat4 rotMat;
        glm::vec3 normVec;
        glm::vec3 centerVec;
        tPlane(glm::mat4 mm, glm::vec3 nv){
            rotMat = mm; normVec = nv;
            rotMat[3][0] = .0f;rotMat[3][1] = .0f;rotMat[3][2] = .0f;
            centerVec = glm::vec3(mm[3][0], mm[3][1], mm[3][2]);
        }
    };
    std::vector<tPlane> getTrackedPlanes(){return tracked_planes;}
private:
    static arController* _myPtr;

    ArSession* ar_session_ = nullptr;
    ArFrame* ar_frame_ = nullptr;
    ArConfig * ar_config_ = nullptr;

    //renders
    backgroundRenderer* bg_render = nullptr;
    PointCloudRenderer* point_cloud_renderer_ = nullptr;
    PlaneRenderer* plane_renderer_ = nullptr;
    lineRenderer* stroke_renderer = nullptr;
    cutplaneRenderer* cutplane_renderer = nullptr;

    ArucoMarkerTracker* m_aruco_tracker;
    //ar camera
    float transformed_uvs_[8];
    bool uvs_initialized_ = false;

    bool install_requested_ = false;
    int width_ = 1;
    int height_ = 1;
    int display_rotation_ = 0;
    glm::mat4 camera_pose_col_major_;

    bool background_tex_initialized = false;
    glm::mat4 view_mat, proj_mat;
    bool m_tracking_needs_reset = false;

    const static int MAX_TRACKED_ANCHORS = 20;
    int anchor_id = 0;
//    ArAnchor* anchors_[MAX_TRACKED_ANCHORS];
    ArAnchor* last_anchor = nullptr;

    //NDK Image
    std::mutex frame_image_in_use_mutex_;
    const uint8_t* m_gray_frame_data = nullptr;
    int m_img_num = 0;
    bool initialized = false;

    std::vector<glm::vec3> plane_vertices_;
    std::vector<GLushort> plane_triangles_;
    std::vector<tPlane> tracked_planes;

    bool onDrawMarkerBased();
    bool onDrawARCoreBased(ArCamera*& camera);
    void update_and_draw_planes();
    void update_plane_vertices(const ArPlane& ar_plane, glm::mat4& model_mat, glm::vec3& normal_vec);
    float get_dist_to_plane(const ArPose& plane_pose, const ArPose& camera_pose);
    glm::vec3 get_plane_norm(const ArPose& plane_pose);
};
#endif
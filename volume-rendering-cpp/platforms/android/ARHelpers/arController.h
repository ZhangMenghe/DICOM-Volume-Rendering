#ifndef AR_CONTROLLER_H
#define AR_CONTROLLER_H
#include <arcore_c_api.h>
#include <platforms/platform.h>
#include <nEntrance.h>
#include <glm/glm.hpp>
#include <platforms/android/Renderers/backgroundRenderer.h>
#include <platforms/android/Renderers/PointCloudRenderer.h>
#include <platforms/android/Renderers/PlaneRenderer.h>

class arController:public nEntrance{
public:
    static arController * instance();
    arController();
    ~arController();

    void onViewChange(int rot, int width, int height);
    void onPause();
    void onResume(void* env, void* context, void* activity);

/*Override*/
    void onViewCreated();
    void onViewChange(int width, int height){}
    void onDraw();
    void onReset(){}
    void onDestroy(){}

    void onSingleTouchDown(float x, float y){ }
    void onTouchMove(float x, float y){}
    void onScale(float sx, float sy){}
    void onPan(float x, float y){}

private:
    static arController* _myPtr;

    ArSession* ar_session_ = nullptr;
    ArFrame* ar_frame_ = nullptr;
    ArConfig * ar_config_ = nullptr;

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

    backgroundRenderer* bg_render = nullptr;
    PointCloudRenderer* point_cloud_renderer_ = nullptr;
    PlaneRenderer* plane_renderer_ = nullptr;

    //plane
    int32_t plane_count_ = 0;
    std::vector<glm::vec3> plane_vertices_;
    std::vector<GLushort> plane_triangles_;

    void update_and_draw_planes();
    void update_plane_vertices(const ArPlane& ar_plane, glm::mat4& model_mat, glm::vec3& normal_vec);
};
#endif
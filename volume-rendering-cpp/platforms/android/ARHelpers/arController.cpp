#include <cstdlib>
#include "arController.h"
#include <glm/gtc/type_ptr.hpp>
#include <dicomRenderer/screenQuad.h>
#include <vrController.h>
#include <glm/gtx/quaternion.hpp>

arController* arController::_myPtr = nullptr;
arController *arController::instance() {
    if(!_myPtr) _myPtr = new arController;
    return _myPtr;
}

arController::arController(){
    _myPtr = this;
}

arController::~arController(){
    if (ar_session_ != nullptr) {
        ArSession_destroy(ar_session_);
        ArFrame_destroy(ar_frame_);
    }
}

void arController::onViewCreated(){
    bg_render = new backgroundRenderer(true);
    point_cloud_renderer_ = new PointCloudRenderer(true);
    plane_renderer_ = new PlaneRenderer(true);
}

void arController::onPause(){
    if (ar_session_ != nullptr) {
        ArSession_pause(ar_session_);
    }
}
void arController::onResume(void* env, void* context, void* activity){
    if(nullptr == ar_session_){
        ArInstallStatus install_status;
        bool user_requested_install = !install_requested_;

        CHECK(ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status) == AR_SUCCESS);

        switch (install_status) {
            case AR_INSTALL_STATUS_INSTALLED:
                break;
            case AR_INSTALL_STATUS_INSTALL_REQUESTED:
                install_requested_ = true;
                return;
        }
        CHECK(ArSession_create(env, context, &ar_session_)==AR_SUCCESS);
        CHECK(ar_session_);
        ArFrame_create(ar_session_, &ar_frame_);
        CHECK(ar_frame_);
        ArSession_setDisplayGeometry(ar_session_, width_, height_, display_rotation_);


        ArCloudAnchorMode out_cloud_anchor_mode;
        ArFocusMode focus_mode;
        ArPlaneFindingMode plane_finding_mode;
        ArUpdateMode update_mode;

        ArConfig_create(ar_session_, &ar_config_);
        CHECK(ar_config_);
        ArConfig_getCloudAnchorMode(ar_session_, ar_config_, &out_cloud_anchor_mode);
        ArConfig_getFocusMode(ar_session_, ar_config_, &focus_mode);
        ArConfig_getPlaneFindingMode(ar_session_, ar_config_, &plane_finding_mode);
        ArConfig_getUpdateMode(ar_session_, ar_config_, &update_mode);

        ArConfig_setFocusMode(ar_session_, ar_config_, AR_FOCUS_MODE_AUTO);
        ArConfig_setUpdateMode(ar_session_, ar_config_, AR_UPDATE_MODE_LATEST_CAMERA_IMAGE);
        CHECK(ArSession_configure(ar_session_, ar_config_) == AR_SUCCESS);
    }

    const ArStatus status = ArSession_resume(ar_session_);
    CHECK(status == AR_SUCCESS);
}

void arController::onViewChange(int rot, int width, int height){
    display_rotation_ = rot;
    width_ = width;
    height_ = height;
    if (ar_session_ != nullptr) {
        ArSession_setDisplayGeometry(ar_session_, rot, width, height);
    }
}
void arController::onDraw(){
    // Render the scene.
//    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
//    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(ar_session_ == nullptr)
        return ;
    if(!background_tex_initialized){
        ArSession_setCameraTextureName(ar_session_, bg_render->GetTextureId());
        background_tex_initialized = true;
    }
    // Update session to get current frame and render camera background.
    if (ArSession_update(ar_session_, ar_frame_) != AR_SUCCESS) {
        LOGE("OnDrawFrame ArSession_update error");
    }

    ArCamera* camera;
    ArFrame_acquireCamera(ar_session_, ar_frame_, &camera);
    ArCamera_getViewMatrix(ar_session_, camera, glm::value_ptr(view_mat));
    ArCamera_getProjectionMatrix(ar_session_, camera, 0.1f, 100.0f, glm::value_ptr(proj_mat));
    vrController::instance()->camera->setProjMat(proj_mat);
    vrController::instance()->camera->setViewMat(view_mat);


    // If the camera isn't tracking don't bother rendering other objects.
    ArTrackingState camera_tracking_state;
    ArCamera_getTrackingState(ar_session_, camera, &camera_tracking_state);


    //draw background
    int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(ar_session_, ar_frame_, &geometry_changed);
    if (geometry_changed != 0 || !uvs_initialized_) {
        ArFrame_transformCoordinates2d(ar_session_, ar_frame_,
                                       AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES, kNumVertices, kVertices,
                                       AR_COORDINATES_2D_TEXTURE_NORMALIZED, transformed_uvs_);
        uvs_initialized_ = true;
    }

    bg_render->dirtyPrecompute();
    bg_render->Draw(transformed_uvs_);

    if (camera_tracking_state != AR_TRACKING_STATE_TRACKING) {

        return ;
    }

    //update and render planes
    update_and_draw_planes();

    // Update and render point cloud.
    ArPointCloud* ar_point_cloud = nullptr;
    ArStatus point_cloud_status =
            ArFrame_acquirePointCloud(ar_session_, ar_frame_, &ar_point_cloud);
    if (point_cloud_status == AR_SUCCESS) {
        int32_t number_of_points = 0;
        const float* point_cloud_data;
        ArPointCloud_getNumberOfPoints(ar_session_, ar_point_cloud, &number_of_points);

        if (number_of_points > 0) {
            ArPointCloud_getData(ar_session_, ar_point_cloud, &point_cloud_data);
        }
        point_cloud_renderer_->Draw(proj_mat * view_mat, number_of_points, point_cloud_data);

        ArPointCloud_release(ar_point_cloud);
    }

    ArPose* camera_pose = nullptr;
    ArPose_create(ar_session_, nullptr, &camera_pose);
    ArCamera_getDisplayOrientedPose(ar_session_, camera, camera_pose);
    ArCamera_release(camera);
    float camera_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, camera_pose, camera_pose_raw);
//    vrController::instance()->camera->setCamPos(glm::vec3(camera_pose_raw[4], camera_pose_raw[5],camera_pose_raw[6]));

    ArPose_getMatrix(ar_session_, camera_pose, glm::value_ptr(camera_pose_col_major_) );
    vrController::instance()->camera->updateCameraPose(camera_pose_col_major_);//(glm::transpose(camera_pose_col_major_));

//    float normal_distance_to_plane = util::CalculateDistanceToPlane(
//            *ar_session_, *hit_pose, *camera_pose);
//
//    ArPose_destroy(hit_pose);
    ArPose_destroy(camera_pose);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

//    //update camera pose
//    ArPose* camera_pose = nullptr;
//    ArPose_create(ar_session_, nullptr, &camera_pose);
//    ArCamera_getPose(ar_session_, camera, camera_pose);
//
//    float camera_pose_raw[7] = {0.f};
////    glm::mat4 camera_pose_col_major;
//    ArPose_getPoseRaw(ar_session_, camera_pose, camera_pose_raw);
//    ArPose_getMatrix(ar_session_, camera_pose, glm::value_ptr(camera_pose_col_major_) );

//    LOGE("===POSE %f, %f, %f, %f,%f, %f, %f", camera_pose_raw[0], camera_pose_raw[1],camera_pose_raw[2],camera_pose_raw[3],camera_pose_raw[4],camera_pose_raw[5],camera_pose_raw[6]);
}
void arController::update_and_draw_planes(){
    // Update and render planes.
    ArTrackableList* plane_list = nullptr;
    ArTrackableList_create(ar_session_, &plane_list);
    CHECK(plane_list != nullptr);

    ArTrackableType plane_tracked_type = AR_TRACKABLE_PLANE;
    ArSession_getAllTrackables(ar_session_, plane_tracked_type, plane_list);

    int32_t plane_list_size = 0;
    ArTrackableList_getSize(ar_session_, plane_list, &plane_list_size);
    plane_count_ = plane_list_size;

    for (int i = 0; i < plane_list_size; ++i) {
        ArTrackable* ar_trackable = nullptr;
        ArTrackableList_acquireItem(ar_session_, plane_list, i, &ar_trackable);
        ArPlane* ar_plane = ArAsPlane(ar_trackable);
        ArTrackingState out_tracking_state;
        ArTrackable_getTrackingState(ar_session_, ar_trackable,
                                     &out_tracking_state);

        ArPlane* subsume_plane;
        ArPlane_acquireSubsumedBy(ar_session_, ar_plane, &subsume_plane);
        if (subsume_plane != nullptr) {
            ArTrackable_release(ArAsTrackable(subsume_plane));
            continue;
        }

        if (ArTrackingState::AR_TRACKING_STATE_TRACKING != out_tracking_state) {
            continue;
        }

        ArTrackingState plane_tracking_state;
        ArTrackable_getTrackingState(ar_session_, ArAsTrackable(ar_plane),
                                     &plane_tracking_state);
        if (plane_tracking_state == AR_TRACKING_STATE_TRACKING) {
            glm::mat4 model_mat; glm::vec3 norm_vec;

            update_plane_vertices(*ar_plane, model_mat, norm_vec);
            plane_renderer_->Draw(plane_vertices_, plane_triangles_, proj_mat * view_mat, model_mat,norm_vec, glm::vec3(1.0f));
            ArTrackable_release(ar_trackable);
        }
    }

    ArTrackableList_destroy(plane_list);
    plane_list = nullptr;
}
void arController::update_plane_vertices(const ArPlane& ar_plane, glm::mat4& model_mat, glm::vec3& normal_vec) {
    // The following code generates a triangle mesh filling a convex polygon,
    // including a feathered edge for blending.
    //
    // The indices shown in the diagram are used in comments below.
    // _______________     0_______________1
    // |             |      |4___________5|
    // |             |      | |         | |
    // |             | =>   | |         | |
    // |             |      | |         | |
    // |             |      |7-----------6|
    // ---------------     3---------------2

    plane_vertices_.clear();
    plane_triangles_.clear();

    int32_t polygon_length;
    ArPlane_getPolygonSize(ar_session_, &ar_plane, &polygon_length);

    if (polygon_length == 0) {
        LOGE("PlaneRenderer::UpdatePlane, no valid plane polygon is found");
        return;
    }

    const int32_t plane_vertices_size = polygon_length / 2;
    std::vector<glm::vec2> raw_vertices(plane_vertices_size);
    ArPlane_getPolygon(ar_session_, &ar_plane,
                       glm::value_ptr(raw_vertices.front()));

    // Fill vertex 0 to 3. Note that the vertex.xy are used for x and z
    // position. vertex.z is used for alpha. The outter polygon's alpha
    // is 0.
    for (int32_t i = 0; i < plane_vertices_size; ++i) {
        plane_vertices_.push_back(glm::vec3(raw_vertices[i].x, raw_vertices[i].y, 0.0f));
    }

    ArPose *pose_;
    ArPose_create(ar_session_, nullptr, &pose_);

    ArPlane_getCenterPose(ar_session_, &ar_plane, pose_);
    ArPose_getMatrix(ar_session_, pose_, glm::value_ptr(model_mat));

    //get normal
    float plane_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, pose_, plane_pose_raw);
    glm::quat plane_quaternion(plane_pose_raw[3], plane_pose_raw[0],
                               plane_pose_raw[1], plane_pose_raw[2]);
    // Get normal vector, normal is defined to be positive Y-position in local
    // frame.
    normal_vec = glm::rotate(plane_quaternion, glm::vec3(0., 1.f, 0.));


    // Feather distance 0.2 meters.
    const float kFeatherLength = 0.2f;
    // Feather scale over the distance between plane center and vertices.
    const float kFeatherScale = 0.2f;

    // Fill vertex 4 to 7, with alpha set to 1.
    for (int32_t i = 0; i < plane_vertices_size; ++i) {
        // Vector from plane center to current point.
        glm::vec2 v = raw_vertices[i];
        const float scale =
                1.0f - std::min((kFeatherLength / glm::length(v)), kFeatherScale);
        const glm::vec2 result_v = scale * v;

        plane_vertices_.push_back(glm::vec3(result_v.x, result_v.y, 1.0f));
    }

    const int32_t plane_vertices_length = plane_vertices_.size();
    const int32_t half_plane_vertices_length = plane_vertices_length / 2;

    // Generate triangle (4, 5, 6) and (4, 6, 7).
    for (int i = half_plane_vertices_length + 1; i < plane_vertices_length - 1; ++i) {
        plane_triangles_.push_back(half_plane_vertices_length);
        plane_triangles_.push_back(i);
        plane_triangles_.push_back(i + 1);
    }

    // Generate triangle (0, 1, 4), (4, 1, 5), (5, 1, 2), (5, 2, 6),
    // (6, 2, 3), (6, 3, 7), (7, 3, 0), (7, 0, 4)
    for (int i = 0; i < half_plane_vertices_length; ++i) {
        plane_triangles_.push_back(i);
        plane_triangles_.push_back((i + 1) % half_plane_vertices_length);
        plane_triangles_.push_back(i + half_plane_vertices_length);

        plane_triangles_.push_back(i + half_plane_vertices_length);
        plane_triangles_.push_back((i + 1) % half_plane_vertices_length);
        plane_triangles_.push_back(
                (i + half_plane_vertices_length + 1) % half_plane_vertices_length +
                half_plane_vertices_length);
    }
}


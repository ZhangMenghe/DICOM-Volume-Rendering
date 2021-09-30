#include <cstdlib>
#include "arController.h"
#include <glm/gtc/type_ptr.hpp>
#include <dicomRenderer/screenQuad.h>
#include <glm/gtx/quaternion.hpp>
#include <Manager.h>
#include <dicomRenderer/Constants.h>
#include <platforms/android/Utils/assetLoader.h>
#include <vrController.h>

arController* arController::_myPtr = nullptr;
arController *arController::instance() {
    if(!_myPtr) _myPtr = new arController;
    return _myPtr;
}

arController::arController(){
    _myPtr = this;
    m_aruco_tracker = new ArucoMarkerTracker;
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
    stroke_renderer = new lineRenderer(true);
    cutplane_renderer = new cutplaneRenderer(true);
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

//        auto status = ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status);
//        CHECK( status== AR_SUCCESS);
//
//        LOGE("=======C1 %d %d", status,install_status);
//        switch (install_status) {
//            case AR_INSTALL_STATUS_INSTALLED:
//                break;
//            case AR_INSTALL_STATUS_INSTALL_REQUESTED:
//                install_requested_ = true;
//                return;
//        }
        LOGE("=======C2");

        try {
            ArSession_create(env, context, &ar_session_);
        } catch (const std::exception& e) {
            LOGI("===== Problem to create ar session");
            // will be executed if f() throws std::runtime_error
        } catch (const std::runtime_error& e){
        LOGI("===== Problem to create ar session");
            // dead code!
        }

//        ArStatus sess_status =
//        if(sess_status!=AR_SUCCESS)
//            LOGI("===== Problem to create ar session");
        ArFrame_create(ar_session_, &ar_frame_);
        CHECK(ar_frame_);
        ArSession_setDisplayGeometry(ar_session_, width_, height_, display_rotation_);

        ArCloudAnchorMode out_cloud_anchor_mode;
        ArFocusMode focus_mode;
        ArPlaneFindingMode plane_finding_mode;
        ArUpdateMode update_mode;
        LOGE("=======C3");

        ArConfig_create(ar_session_, &ar_config_);
        CHECK(ar_config_);
        ArConfig_getCloudAnchorMode(ar_session_, ar_config_, &out_cloud_anchor_mode);
        ArConfig_getFocusMode(ar_session_, ar_config_, &focus_mode);
        ArConfig_getPlaneFindingMode(ar_session_, ar_config_, &plane_finding_mode);
        ArConfig_getUpdateMode(ar_session_, ar_config_, &update_mode);

        ArConfig_setFocusMode(ar_session_, ar_config_, AR_FOCUS_MODE_AUTO);
        ArConfig_setUpdateMode(ar_session_, ar_config_, AR_UPDATE_MODE_LATEST_CAMERA_IMAGE);
        CHECK(ArSession_configure(ar_session_, ar_config_) == AR_SUCCESS);
        LOGE("=======C4");
    }

    const ArStatus status = ArSession_resume(ar_session_);
    LOGE("=======C5 %d", status);
}

void arController::onViewChange(int rot, int width, int height){
    display_rotation_ = rot;
    width_ = width;
    height_ = height;
    if (ar_session_ != nullptr) {
        ArSession_setDisplayGeometry(ar_session_, rot, width, height);
    }
}
bool arController::onDrawMarkerBased(){
    std::lock_guard<std::mutex> lock(frame_image_in_use_mutex_);
    ArImage * ar_image;
    ArStatus status = ArFrame_acquireCameraImage(ar_session_, ar_frame_, &ar_image);
    if(ar_image == nullptr || status != AR_SUCCESS) {
        return false;
    }

    if(!initialized) {
        ArImageFormat format;
        int32_t ndk_image_width=0, ndk_image_height=0, num_plane = 0, stride = 0;

        ArImage_getFormat(ar_session_, ar_image, &format);
        if (format != AR_IMAGE_FORMAT_YUV_420_888) {
            LOGE("===Format error: AR_IMAGE_FORMAT_YUV_420_888 needed ");
            ArImage_release(ar_image);
            return false;
        }

        ArImage_getWidth(ar_session_, ar_image, &ndk_image_width);
        ArImage_getHeight(ar_session_, ar_image, &ndk_image_height);
        ArImage_getNumberOfPlanes(ar_session_, ar_image, &num_plane);
        ArImage_getPlaneRowStride(ar_session_, ar_image, 0, &stride);

        if (ndk_image_width <= 0 || ndk_image_height <= 0 || num_plane <= 0 || stride <= 0) {
            LOGE("===Fail to get ndk image");
            ArImage_release(ar_image);
            return false;
        }
        m_aruco_tracker->setImageSize(ndk_image_width, ndk_image_height);
        initialized = true;
    }

    int32_t length = 0;
    ArImage_getPlaneData(ar_session_, ar_image, 0, &m_gray_frame_data, &length);

    m_aruco_tracker->Update(m_gray_frame_data);
    ArImage_release(ar_image);
    m_lasttime_arcore = false;
    return true;
}
bool arController::onDrawARCoreBased(ArCamera*& camera){
// If the camera isn't tracking don't bother rendering other objects.
    ArTrackingState camera_tracking_state;
    ArCamera_getTrackingState(ar_session_, camera, &camera_tracking_state);

    if (camera_tracking_state != AR_TRACKING_STATE_TRACKING) return false;

    //update and render planes
    update_and_draw_planes();
    if(!m_lasttime_arcore && !tracked_planes.empty())
        vrController::instance()->setVolumeRST(tracked_planes[0].rotMat, glm::vec3(0.2f), tracked_planes[0].centerVec);

    auto mVP = Manager::camera->getVPMat();
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
        if(Manager::param_bool[dvr::CHECK_AR_DRAW_POINT])
            point_cloud_renderer_->Draw(mVP, number_of_points, point_cloud_data);

        ArPointCloud_release(ar_point_cloud);
    }
    //draw stroke
    stroke_renderer->Draw(mVP);
    //draw cutting plane
    cutplane_renderer->Draw(mVP, stroke_renderer->getEndPosWorld(), Manager::camera->getViewDirection());
    m_lasttime_arcore = true;
    return true;
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
        return;


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
    Manager::camera->setProjMat(proj_mat);
    if(Manager::param_bool[dvr::CHECK_AR_USE_ARCORE]) Manager::camera->setViewMat(view_mat);

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

    if(Manager::param_bool[dvr::CHECK_AR_USE_ARCORE]) onDrawARCoreBased(camera);
    else onDrawMarkerBased();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
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
            tracked_planes.push_back(tPlane(model_mat, norm_vec));
            if(Manager::param_bool[dvr::CHECK_AR_DRAW_PLANE])
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
    normal_vec = get_plane_norm(*pose_);

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
glm::vec3 arController::get_plane_norm(const ArPose& plane_pose){
    float plane_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, &plane_pose, plane_pose_raw);
    glm::quat plane_quaternion(plane_pose_raw[3], plane_pose_raw[0],
                               plane_pose_raw[1], plane_pose_raw[2]);
    // Get normal vector, normal is defined to be positive Y-position in local
    // frame.
    return glm::rotate(plane_quaternion, glm::vec3(0., 1.f, 0.));
}
float arController::get_dist_to_plane(const ArPose& plane_pose, const ArPose& camera_pose){
    float plane_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, &plane_pose, plane_pose_raw);
    glm::vec3 plane_position(plane_pose_raw[4], plane_pose_raw[5],
                             plane_pose_raw[6]);
    glm::vec3 normal = get_plane_norm(plane_pose);

    float camera_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, &camera_pose, camera_pose_raw);
    glm::vec3 camera_P_plane(camera_pose_raw[4] - plane_position.x,
                             camera_pose_raw[5] - plane_position.y,
                             camera_pose_raw[6] - plane_position.z);
    return glm::dot(normal, camera_P_plane);
}

bool arController::onLongPress(float x, float y){
    if (!Manager::param_bool[dvr::CHECK_AR_USE_ARCORE] || ar_frame_ == nullptr || ar_session_ == nullptr) return false;

    ArHitResultList* hit_result_list = nullptr;
    ArHitResultList_create(ar_session_, &hit_result_list);
    CHECK(hit_result_list);
    ArFrame_hitTest(ar_session_, ar_frame_, x, y, hit_result_list);

    int32_t hit_result_list_size = 0;
    ArHitResultList_getSize(ar_session_, hit_result_list,
                            &hit_result_list_size);

    // The hitTest method sorts the resulting list by distance from the camera,
    // increasing.  The first hit result will usually be the most relevant when
    // responding to user input.

    ArHitResult* ar_hit_result = nullptr;
    ArTrackableType trackable_type = AR_TRACKABLE_NOT_VALID;
    for (int32_t i = 0; i < hit_result_list_size; ++i) {
        ArHitResult* ar_hit = nullptr;
        ArHitResult_create(ar_session_, &ar_hit);
        ArHitResultList_getItem(ar_session_, hit_result_list, i, ar_hit);

        if (ar_hit == nullptr) {
            LOGE("HelloArApplication::OnTouched ArHitResultList_getItem error");
            return false;
        }

        ArTrackable* ar_trackable = nullptr;
        ArHitResult_acquireTrackable(ar_session_, ar_hit, &ar_trackable);
        ArTrackableType ar_trackable_type = AR_TRACKABLE_NOT_VALID;
        ArTrackable_getType(ar_session_, ar_trackable, &ar_trackable_type);
        // Creates an anchor if a plane or an oriented point was hit.
        if (AR_TRACKABLE_PLANE != ar_trackable_type) continue;

        ArPose* hit_pose = nullptr;
        ArPose_create(ar_session_, nullptr, &hit_pose);
        ArHitResult_getHitPose(ar_session_, ar_hit, hit_pose);
        int32_t in_polygon = 0;
        ArPlane* ar_plane = ArAsPlane(ar_trackable);
        ArPlane_isPoseInPolygon(ar_session_, ar_plane, hit_pose, &in_polygon);

        // Use hit pose and camera pose to check if hittest is from the
        // back of the plane, if it is, no need to create the anchor.
        ArPose* camera_pose = nullptr;
        ArPose_create(ar_session_, nullptr, &camera_pose);
        ArCamera* ar_camera;
        ArFrame_acquireCamera(ar_session_, ar_frame_, &ar_camera);
        ArCamera_getPose(ar_session_, ar_camera, camera_pose);
        ArCamera_release(ar_camera);
        float normal_distance_to_plane = get_dist_to_plane(*hit_pose, *camera_pose);

        ArPose_destroy(hit_pose);
        ArPose_destroy(camera_pose);

        if (!in_polygon || normal_distance_to_plane < 0) {
            continue;
        }
        ar_hit_result = ar_hit;
    }

    if (ar_hit_result) {
        // Note that the application is responsible for releasing the anchor
        // pointer after using it. Call ArAnchor_release(anchor) to release.
        ArAnchor* anchor = nullptr;
        if (ArHitResult_acquireNewAnchor(ar_session_, ar_hit_result, &anchor) != AR_SUCCESS) {
            LOGE("HelloArApplication::OnTouched ArHitResult_acquireNewAnchor error");
            return false;
        }

        ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
        ArAnchor_getTrackingState(ar_session_, anchor, &tracking_state);
        if (tracking_state != AR_TRACKING_STATE_TRACKING) {
            ArAnchor_release(anchor);
            return false;
        }
        last_anchor = anchor;

        ArHitResultList_destroy(hit_result_list);
        hit_result_list = nullptr;
        return true;
    }
    return false;
}
void arController::getTouchedAnchor(glm::mat4& rotMat, glm::vec3& pos){
    if(last_anchor == nullptr)return;
    glm::mat4 mmat;
    ArPose* anchor_pose = nullptr;
    ArPose_create(ar_session_, nullptr, &anchor_pose);
    ArAnchor_getPose(ar_session_, last_anchor, anchor_pose);
    ArPose_getMatrix(ar_session_, anchor_pose, glm::value_ptr(mmat));
    ArPose_destroy(anchor_pose);
    pos = glm::vec3(mmat[0][3],mmat[1][3],mmat[2][3] );
    rotMat = mmat;
    rotMat[0][3] = rotMat[1][3] = rotMat[2][3] = .0f;
}
bool arController::getTouchedPosition(glm::vec3& pos){
    if(last_anchor == nullptr)return false;
    ArPose* anchor_pose = nullptr;
    ArPose_create(ar_session_, nullptr, &anchor_pose);
    ArAnchor_getPose(ar_session_, last_anchor, anchor_pose);
    float pose_raw[7] = {0.f};
    ArPose_getPoseRaw(ar_session_, anchor_pose, pose_raw);
    pos = glm::vec3(pose_raw[4],pose_raw[5],pose_raw[6]);
    ArPose_destroy(anchor_pose);
    return true;
}


void arController::onReset(){
    //todo:set reset values
}

void arController::onSingleTouchDown(float x, float y){
    if(dvr::AR_CAPTURE_FRAMES){
        if(assetLoader::instance()->saveDataToAndroidExternalStorage(m_gray_frame_data))
            LOGE("=====WRITE SUCCESS %d", m_img_num++);
        else
            LOGE("=====WRITE FAILED");
    }else{
        //perform raycast
        stroke_renderer->setStartPoint(x,y);
        Manager::show_ar_ray = true;
    }
}
void arController::onSingleTouchUp(){
    Manager::show_ar_ray = false;
    Manager::volume_ar_hold = false;
}
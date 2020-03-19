#include <cstdlib>
#include "arController.h"
#include <glm/gtc/type_ptr.hpp>
//namespace {
//// Positions of the quad vertices in clip space (X, Y).
//    const GLfloat kVertices[] = {
//            -1.0f, -1.0f,
//            +1.0f, -1.0f,
//            -1.0f, +1.0f,
//            +1.0f, +1.0f,
//    };
//}
arController* arController::_myPtr = nullptr;
arController *arController::instance() {
    if(!_myPtr) _myPtr = new arController;
    return _myPtr;
}

arController::arController(){

}

arController::~arController(){
    if (ar_session_ != nullptr) {
        ArSession_destroy(ar_session_);
        ArFrame_destroy(ar_frame_);
    }
}

void arController::onViewCreated(){
    bg_render = new backgroundRenderer;
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
    //ArSession_setCameraTextureName(ar_session_, bgTextureId);
    // Update session to get current frame and render camera background.
    if (ArSession_update(ar_session_, ar_frame_) != AR_SUCCESS) {
        LOGE("OnDrawFrame ArSession_update error");
    }

    ArCamera* camera;
    ArFrame_acquireCamera(ar_session_, ar_frame_, &camera);
    ArCamera_getViewMatrix(ar_session_, camera, glm::value_ptr(view_mat));
    ArCamera_getProjectionMatrix(ar_session_,camera, 0.1f, 100.0f, glm::value_ptr(proj_mat));

    // If the camera isn't tracking don't bother rendering other objects.
    ArTrackingState camera_tracking_state;
    ArCamera_getTrackingState(ar_session_, camera, &camera_tracking_state);
    ArCamera_release(camera);

    //draw background
    int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(ar_session_, ar_frame_, &geometry_changed);
    if (geometry_changed != 0 || !uvs_initialized_) {
        ArFrame_transformCoordinates2d(ar_session_, ar_frame_,
                                       AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES, kNumVertices, kVertices,
                                       AR_COORDINATES_2D_TEXTURE_NORMALIZED, transformed_uvs_);
        uvs_initialized_ = true;
    }

    bg_render->Draw(transformed_uvs_);

    if (camera_tracking_state != AR_TRACKING_STATE_TRACKING) {
        return ;
    }

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

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

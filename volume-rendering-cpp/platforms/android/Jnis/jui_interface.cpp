#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jui_interface.h"
#include <vrController.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <platforms/android/ARHelpers/arController.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;

//current only opacity has multiple widgets
JUI_METHOD(void, JUIaddTuneParamsNative)(JNIEnv * env, jclass, jintArray jnums, jfloatArray jvalues){
    jint* nums = env->GetIntArrayElements(jnums, 0);
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
//    overlayController::instance()->addWidget(std::vector<float>(values, values+nums[0]));
    m_manager->addOpacityWidget(std::vector<float>(values, values+nums[0]));
    env->ReleaseFloatArrayElements(jvalues, values, 0);
    env->ReleaseIntArrayElements(jnums, nums, 0);
}
void InitCheckParam(JNIEnv * env, jint num, jobjectArray jkeys, jbooleanArray jvalues){
    std::vector<std::string> keys;
    std::vector<bool> values;

    jboolean* jvalue_arr = env->GetBooleanArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        auto jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        keys.push_back(dvr::jstring2string(env,jkey));
        values.push_back(jvalue_arr[i]);
        LOGE("======SET INIT %s, %s", keys.back().c_str(), values[i]?"true":"false");
    }
    env->ReleaseBooleanArrayElements(jvalues,jvalue_arr,0);

    m_manager->InitCheckParams(keys, values);
    m_manager->addMVPStatus("ARCam", false);
}

JUI_METHOD(void, JUIsetTuneWidgetByIdNative)(JNIEnv *, jclass, jint wid){
    m_manager->setOpacityWidgetId(wid);
}
JUI_METHOD(void, JUIremoveTuneWidgetByIdNative)(JNIEnv *, jclass, jint wid){
    m_manager->removeOpacityWidget(wid);
}
JUI_METHOD(void, JUIremoveAllTuneWidgetNative)(JNIEnv *, jclass){
    m_manager->removeAllOpacityWidgets();
}
JUI_METHOD(void, JUIsetTuneParamByIdNative)(JNIEnv *, jclass, jint tid, jint pid, jfloat value){
    if(tid == dvr::TID_OPACITY){
        if(pid < dvr::TUNE_END)m_manager->setOpacityValue(pid, value);
    }
    else if(tid == dvr::TID_CONTRAST) m_manager->setRenderParam(pid, value);
    else{
        jfloat tmp[] = {value};
        m_sceneRenderer->setRenderingParameters(dvr::RENDER_METHOD(tid-2), tmp);
    }
}
JUI_METHOD(void, JUIsetDualParamByIdNative)(JNIEnv *, jclass, jint pid, jfloat minv, jfloat maxv){
//    if(pid < dvr::DUAL_END)m_sceneRenderer->setDualParameter(pid, minv, maxv);
}
JUI_METHOD(void, JUIsetChecksNative)(JNIEnv * env, jclass, jstring jkey, jboolean value){
    m_manager->setCheck(dvr::jstring2string(env,jkey), value);
}
JUI_METHOD(void, JUISwitchCuttingPlaneNative)(JNIEnv * env, jclass, jint id){
    m_sceneRenderer->SwitchCuttingPlane((PARAM_CUT_ID)id);
}
JUI_METHOD(void, JUIsetOnChangeRecordingStatus)(JNIEnv * env, jclass, jboolean is_recording){
    if(!Manager::param_bool[dvr::CHECK_AR_ENABLED]) m_sceneRenderer->setPredrawOption(is_recording);

    screenQuad::instance()->setOnChangeRecordingStatus(is_recording);
}
JUI_METHOD(jfloatArray, JUIgetVCStatesNative)(JNIEnv * env, jclass){
    jfloatArray res = env->NewFloatArray(31);
    env->SetFloatArrayRegion(res,0,31, reinterpret_cast<jfloat *>(m_sceneRenderer->getCurrentReservedStates()));
    return res;
}
JUI_METHOD(jfloatArray, JUIgetCuttingPlaneStatusNative)(JNIEnv * env, jclass){
    jfloatArray res= env->NewFloatArray(7);
    env->SetFloatArrayRegion(res,0,7, reinterpret_cast<jfloat *>(m_sceneRenderer->getCuttingPlane()));
    return res;
}
JUI_METHOD(void, JUIsetCuttingPlaneNative)(JNIEnv *, jclass, jfloat value){
    m_sceneRenderer->setCuttingPlane(value);
}
JUI_METHOD(void, JUIsetCuttingPlaneDeltaNative)(JNIEnv * env, jclass, jint id, jint delta){
    m_sceneRenderer->setCuttingPlane(id, delta);
}
JUI_METHOD(void, JUIsetMaskBitsNative)(JNIEnv * env, jclass, jint num, jint mbits){
    m_manager->setMask(num, mbits);
}
JUI_METHOD(void, JUIsetColorSchemeNative)(JNIEnv * env, jclass, jint id){
    m_manager->setColorScheme(id);
//    Manager::color_scheme_id = id;
//    Manager::baked_dirty_ = true;
}
JUI_METHOD(void, JUIsetCLAHEOptionNative)(JNIEnv * env, jclass, jint id){
    m_sceneRenderer->setCLAHEOption(id);
}
JUI_METHOD(void, JUIsetCLAHEVariableDeltaStepNative)(JNIEnv * env, jclass, jboolean up, jint var_id, jint var_sub_id){
    m_sceneRenderer->setCLAHEVariableDeltaStep(up, (dvr::CLAHE_VARIABLES)var_id, var_sub_id);
}
JUI_METHOD(void, JUIApplyCLAHEChanges)(JNIEnv * env, jclass){
    m_sceneRenderer->ApplyCLAHEChanges();
}
JUI_METHOD(void, JUIsetTraversalTargetNative)(JNIEnv * env, jclass, jint id){
    Manager::traversal_target_id = (id == 0)?dvr::ORGAN_COLON:dvr::ORGAN_ILEUM;
    LOGE("====ID: %d", id);
}
JUI_METHOD(void, JUIsetRenderingMethodNative)(JNIEnv * env, jclass, jint id){
    m_sceneRenderer->setRenderingMethod(dvr::RENDER_METHOD(id));
}
JUI_METHOD(void, JUIsetGraphRectNative)(JNIEnv*, jclass, jint id, jint width, jint height, jint left, jint top){
//    LOGE("====%d, %d, %d, %d,%d", id, width, height, left,top);
    m_sceneRenderer->setOverlayRects(id, width, height, left, top);
}
JUI_METHOD(void, JUIsetAllTuneParamByIdNative)(JNIEnv* env, jclass, jint tid, jfloatArray jvalues){
    if (tid >= dvr::TID_END) return;

    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    if(tid == dvr::TID_CONTRAST)m_manager->setRenderParam(values);
    else if(tid == dvr::TID_CUTTING_PLANE)
        m_sceneRenderer->setCuttingPlane(glm::vec3(values[0], values[1], values[2]), glm::vec3(values[3], values[4],values[5]));
    else
        m_sceneRenderer->setRenderingParameters(dvr::RENDER_METHOD(tid-2), values);

    env->ReleaseFloatArrayElements(jvalues,values,0);
}
JUI_METHOD(void, JUIsetTuneWidgetVisibilityNative)(JNIEnv*, jclass, jint wid, jboolean value){
    m_manager->setOpacityWidgetVisibility(wid,value);
    //    overlayController::instance()->setWidgetsVisibility(wid, value);
//    Manager::baked_dirty_ = true;
}
JUI_METHOD(void, JUIonResetNative)(JNIEnv* env, jclass,
        jint num, jobjectArray jkeys, jbooleanArray jvalues,
        jfloatArray jvol_pose, jfloatArray jcam_pose){
    //todo:how to reset??
//    m_manager->onReset();
    InitCheckParam(env, num, jkeys, jvalues);
    jfloat* vol_arr = env->GetFloatArrayElements(jvol_pose, 0);
    jfloat* cam_arr = env->GetFloatArrayElements(jcam_pose, 0);

    glm::mat4 rot_mat = glm::mat4(1.0);
//    memcpy(glm::value_ptr(rot_mat), &vol_arr[6], 16* sizeof(float));
    int id = 6;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            rot_mat[i][j] = vol_arr[id++];
        }
    }
    Camera cam;
    cam.Reset(                    glm::vec3(cam_arr[0], cam_arr[1], cam_arr[2]),
                                  glm::vec3(cam_arr[3], cam_arr[4], cam_arr[5]),
                                  glm::vec3(cam_arr[6], cam_arr[7], cam_arr[8])
                                  );
    //unwrap pose information
    m_sceneRenderer->onReset(
            glm::vec3(vol_arr[0], vol_arr[1], vol_arr[2]),
            glm::vec3(vol_arr[3], vol_arr[4], vol_arr[5]),
            rot_mat,
            &cam);

    env->ReleaseFloatArrayElements(jvol_pose, vol_arr, 0);
    env->ReleaseFloatArrayElements(jcam_pose, cam_arr, 0);
}
//volume array: RST in R(w,x,y,z), S(x,y,z), T(x,y,z)
JUI_METHOD(void, JUIsetVolumePose)(JNIEnv * env, jclass, jbooleanArray jvol_pose_type, jfloatArray jvol_pose){
    jboolean * type_arr = env->GetBooleanArrayElements(jvol_pose_type, 0);
    jfloat* vol_arr = env->GetFloatArrayElements(jvol_pose, 0);
    //unwrap pose information
    m_sceneRenderer->setVolumeRST(
            glm::mat4_cast(glm::quat(vol_arr[0], vol_arr[1], vol_arr[2], vol_arr[3])),
            glm::vec3(vol_arr[4], vol_arr[5], vol_arr[6]),
            glm::vec3(vol_arr[7], vol_arr[8], vol_arr[9]),
            type_arr[0], type_arr[1], type_arr[2]);
    env->ReleaseFloatArrayElements(jvol_pose, vol_arr, 0);
    env->ReleaseBooleanArrayElements(jvol_pose_type, type_arr, 0);
}
JUI_METHOD(void, JUIonSingleTouchDownNative)(JNIEnv *, jclass, jint target, jfloat x, jfloat y){
    if(target == TOUCH_VOLUME) m_sceneRenderer->onSingleTouchDown(x, y);
    else if(target == TOUCH_AR_BUTTON) arController::instance()->onSingleTouchDown(x,y);
}
JUI_METHOD(void, JUIonSingleTouchUpNative)(JNIEnv *, jobject){
    arController::instance()->onSingleTouchUp();
}
JUI_METHOD(void, JUIonTouchMoveNative)(JNIEnv *, jclass, jfloat x, jfloat y){
    m_sceneRenderer->onTouchMove(x, y);
}
JUI_METHOD(void, JUIonScaleNative)(JNIEnv *, jclass, jfloat sx, jfloat sy){
    m_sceneRenderer->onScale(sx, sy);
}
JUI_METHOD(void, JUIonPanNative)(JNIEnv *, jclass, jfloat x, jfloat y){
    m_sceneRenderer->onPan(x,y);
}
JUI_METHOD(void, JUIonLongPress)(JNIEnv *env, jobject obj, jfloat x, jfloat y){
    if(arController::instance()->onLongPress(x,y)){
        //call back to java
        jclass cls = env->FindClass("helmsley/vr/DUIs/arUIs");
        cls = static_cast<jclass>(env->NewGlobalRef(cls));
        jmethodID mid = env->GetStaticMethodID( cls, "ShowPopMenu", "(FF)V");
        if (mid == 0) {
            return;
        }
        env->CallStaticVoidMethod(cls, mid, x, y);
    }
}
JUI_METHOD(void, JUIonARRequest)(JNIEnv * env, jclass, jint id){
    if(id == dvr::PLACE_VOLUME){
        glm::vec3 pos;
        if(arController::instance()->getTouchedPosition(pos));
            m_sceneRenderer->setVolumePosition(pos);
    }
}
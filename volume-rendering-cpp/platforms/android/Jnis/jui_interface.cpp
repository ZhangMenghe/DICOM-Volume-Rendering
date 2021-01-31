#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jui_interface.h"
#include <vrController.h>
#include <overlayController.h>
#include <glm/gtc/type_ptr.hpp>
#include <platforms/android/ARHelpers/arController.h>

using namespace dvr;

namespace {
    std::vector<std::string> param_checks;
}
//current only opacity has multiple widgets
JUI_METHOD(void, JUIaddTuneParamsNative)(JNIEnv * env, jclass, jintArray jnums, jfloatArray jvalues){
    jint* nums = env->GetIntArrayElements(jnums, 0);
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    overlayController::instance()->addWidget(std::vector<float>(values, values+nums[0]));

    env->ReleaseFloatArrayElements(jvalues, values, 0);
    env->ReleaseIntArrayElements(jnums, nums, 0);
}
void InitCheckParam(JNIEnv * env, jint num, jobjectArray jkeys, jbooleanArray jvalues){
    param_checks.clear();
    Manager::param_bool.clear();
    jboolean* values = env->GetBooleanArrayElements(jvalues, 0);
    for(int i=0; i<num; i++){
        jstring jkey = (jstring) (env->GetObjectArrayElement(jkeys, i));
        std::string key = dvr::jstring2string(env,jkey);
        param_checks.push_back(key);
        Manager::param_bool.push_back(values[i]);
        LOGE("======SET INIT %s, %d", key.c_str(), values[i]);
    }
    env->ReleaseBooleanArrayElements(jvalues,values,0);
    Manager::baked_dirty_ = true;
    m_sceneRenderer->addStatus("ARCam");
    camera_switch_dirty = true;
}

JUI_METHOD(void, JUIsetTuneWidgetByIdNative)(JNIEnv *, jclass, jint wid){
    overlayController::instance()->setWidgetId(wid);
}
JUI_METHOD(void, JUIremoveTuneWidgetByIdNative)(JNIEnv *, jclass, jint wid){
    overlayController::instance()->removeWidget(wid);
}
JUI_METHOD(void, JUIremoveAllTuneWidgetNative)(JNIEnv *, jclass){
    overlayController::instance()->removeAll();
}
JUI_METHOD(void, JUIsetTuneParamByIdNative)(JNIEnv *, jclass, jint tid, jint pid, jfloat value){
    if(tid == 0 && pid < dvr::TUNE_END)overlayController::instance()->setTuneParameter(pid, value);
    else if(tid == 1) m_sceneRenderer->setRenderParam(pid, value);
}
JUI_METHOD(void, JUIsetDualParamByIdNative)(JNIEnv *, jclass, jint pid, jfloat minv, jfloat maxv){
    if(pid < dvr::DUAL_END)m_sceneRenderer->setDualParameter(pid, minv, maxv);
}
JUI_METHOD(void, JUIsetChecksNative)(JNIEnv * env, jclass, jstring jkey, jboolean value){
    std::string key = dvr::jstring2string(env,jkey);

    auto it = std::find (param_checks.begin(), param_checks.end(), key);
    if (it != param_checks.end()){
        int bpos = (int)(it - param_checks.begin());
        Manager::param_bool[bpos] = value;
        if(bpos == CHECK_AR_ENABLED)
            camera_switch_dirty = true;
        Manager::baked_dirty_ = true;
    }
}
JUI_METHOD(void, JUISwitchCuttingPlaneNative)(JNIEnv * env, jclass, jint id){
    m_sceneRenderer->SwitchCuttingPlane((PARAM_CUT_ID)id);
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
    m_sceneRenderer->mask_num_ = (unsigned int)num;
    m_sceneRenderer->mask_bits_ = (unsigned int)mbits;
    Manager::baked_dirty_ = true;
}
JUI_METHOD(void, JUIsetColorSchemeNative)(JNIEnv * env, jclass, jint id){
    Manager::color_scheme_id = id;
    Manager::baked_dirty_ = true;
}
JUI_METHOD(void, JUIsetTraversalTargetNative)(JNIEnv * env, jclass, jint id){
    Manager::traversal_target_id = (id == 0)?dvr::ORGAN_COLON:dvr::ORGAN_ILEUM;
    LOGE("====ID: %d", id);
}
JUI_METHOD(void, JUIsetGraphRectNative)(JNIEnv*, jclass, jint id, jint width, jint height, jint left, jint top){
//    LOGE("====%d, %d, %d, %d,%d", id, width, height, left,top);
    overlayController::instance()->setOverlayRect(id, width, height, left, top);
}
JUI_METHOD(void, JUIsetAllTuneParamByIdNative)(JNIEnv* env, jclass, jint id, jfloatArray jvalues){
    jfloat* values = env->GetFloatArrayElements(jvalues, 0);
    if(id == 1)m_sceneRenderer->setRenderParam(values);
    else if(id == 2)m_sceneRenderer->setCuttingPlane(glm::vec3(values[0], values[1], values[2]), glm::vec3(values[3], values[4],values[5]));

    env->ReleaseFloatArrayElements(jvalues,values,0);
}
JUI_METHOD(void, JUIsetTuneWidgetVisibilityNative)(JNIEnv*, jclass, jint wid, jboolean value){
    overlayController::instance()->setWidgetsVisibility(wid, value);
    Manager::baked_dirty_ = true;
}
JUI_METHOD(void, JUIonResetNative)(JNIEnv* env, jclass,
        jint num, jobjectArray jkeys, jbooleanArray jvalues,
        jfloatArray jvol_pose, jfloatArray jcam_pose){
    m_manager->onReset();
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
    //unwrap pose information
    m_sceneRenderer->onReset(
            glm::vec3(vol_arr[0], vol_arr[1], vol_arr[2]),
            glm::vec3(vol_arr[3], vol_arr[4], vol_arr[5]),
            rot_mat,
            new Camera(
                    glm::vec3(cam_arr[0], cam_arr[1], cam_arr[2]),
                    glm::vec3(cam_arr[3], cam_arr[4], cam_arr[5]),
                    glm::vec3(cam_arr[6], cam_arr[7], cam_arr[8])
            ));

    env->ReleaseFloatArrayElements(jvol_pose, vol_arr, 0);
    env->ReleaseFloatArrayElements(jcam_pose, cam_arr, 0);
}
JUI_METHOD(void, JUIonSingleTouchDownNative)(JNIEnv *, jclass, jint target, jfloat x, jfloat y){
    if(target == TOUCH_VOLUME) m_sceneRenderer->onSingleTouchDown(x, y);
    else if(target == TOUCH_AR_BUTTON) arController::instance()->onSingleTouchDown(x,y);
}
JUI_METHOD(void, JUIonSingleTouchUpNative)(JNIEnv *, jobject){
    arController::instance()->onSingleTouchUp();
}
JUI_METHOD(void, JUIonTouchMoveNative)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onTouchMove(x, y);
}
JUI_METHOD(void, JUIonScaleNative)(JNIEnv *, jclass, jfloat sx, jfloat sy){
    nativeApp(nativeAddr)->onScale(sx, sy);
}
JUI_METHOD(void, JUIonPanNative)(JNIEnv *, jclass, jfloat x, jfloat y){
    nativeApp(nativeAddr)->onPan(x,y);
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
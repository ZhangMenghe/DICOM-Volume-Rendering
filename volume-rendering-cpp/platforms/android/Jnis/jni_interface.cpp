#include <Utils/assetLoader.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include "vrController.h"

#include <android/bitmap.h>
#include <vector>
#include <platforms/android/ARHelpers/arController.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;
namespace {
    const int LOAD_DCMI_ID = 0, LOAD_MASK_ID = 1;
    const int LOAD_CENTERLINE_ID=0;
    int CHANEL_NUM = 4;
    //globally
    GLubyte* g_VolumeTexData = nullptr;
    int return_pos = 0;
    std::unordered_map<int, float*> centerline_map;

    int g_img_h=0, g_img_w=0, g_img_d=0;
    float g_vol_h, g_vol_w, g_vol_depth = 0;
    size_t g_ssize = 0, g_vol_len;
    size_t n_data_offset[3] = {0};
    bool arInitialized = false;

    glm::vec4 ray_dir;
    bool ray_initialized = false;

    void setupShaderContents(){
        vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
        const char* shader_file_names[18] = {
                "shaders/textureVolume.vert",
                "shaders/textureVolume.frag",
                "shaders/raycastVolume.vert",
                "shaders/raycastVolume.frag",
                "shaders/raycastVolume.glsl",
                "shaders/raycastCompute.glsl",
                "shaders/quad.vert",
                "shaders/quad.frag",
                "shaders/cplane.vert",
                "shaders/cplane.frag",
                "shaders/colorViz.vert",
//                "shaders/colorViz.frag",
                "shaders/colorTransferCompute.glsl",
                "shaders/opaViz.vert",
                "shaders/opaViz.frag",
                "shaders/marching_cube.glsl",
                "shaders/marching_cube_clear.glsl",
                "shaders/marching_cube_draw.vert",
                "shaders/marching_cube_draw.frag"
        };
        const char* android_shader_file_names[9] = {
                "shaders/arcore_screen_quad.vert",
                "shaders/arcore_screen_quad.frag",
                "shaders/pointcloud.vert",
                "shaders/pointcloud.frag",
                "shaders/plane.vert",
                "shaders/plane.frag",
                "shaders/naive2d.vert",
                "shaders/ar_cutplane.vert",
                "shaders/ar_cutplane.frag"
        };
        for(int i = 0; i<int(dvr::SHADER_END); i++){
            std::string content;
            assetLoader::instance()->LoadTextFileFromAssetManager(shader_file_names[i], &content);
            vrc->setShaderContents(SHADER_FILES (i), content);
        }

        for(int i=0; i<int(dvr::SHADER_ANDROID_END) - int(dvr::SHADER_END); i++){
            std::string content;
            assetLoader::instance()->LoadTextFileFromAssetManager(android_shader_file_names[i], &content);
            Manager::shader_contents[dvr::SHADER_END + i] = content;
        }
    }
}
jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    new assetLoader(AAssetManager_fromJava(env, asset_manager));

    m_manager = std::make_shared<Manager>();
    m_sceneRenderer = std::make_unique<vrController>(m_manager);
    nativeAddr = getNativeClassAddr(m_sceneRenderer.get());

    m_sceneRenderer->setPredrawOption(true);
    setupShaderContents();
    return nativeAddr;
}
JNI_METHOD(void, JNIonPause)(JNIEnv*, jclass){
    m_sceneRenderer->onPause();
    arController::instance()->onPause();
}

JNI_METHOD(void, JNIonDestroy)(JNIEnv*, jclass){
    m_sceneRenderer->onDestroy();
    arController::instance()->onDestroy();
    delete nativeApp(nativeAddr);
    delete arController::instance();

    nativeAddr = 0;
}

JNI_METHOD(void, JNIonResume)(JNIEnv* env, jclass, jobject context, jobject activity){
    m_sceneRenderer->onResume(env, context, activity);
    arController::instance()->onResume(env, context, activity);
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
    m_sceneRenderer->onViewCreated();
//    overlayController::instance()->onViewCreated();
    arController::instance()->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint rot, jint w, jint h){
    m_manager->onViewChange(w, h);
    m_sceneRenderer->onViewChange(w, h);
//    overlayController::instance()->onViewChange(w, h);
    arController::instance()->onViewChange(rot,w,h);
}

void on_draw_native(){
    if(Manager::show_ar_ray && !Manager::volume_ar_hold){
        //check ar ray intersect
        Camera* cam = Manager::camera;
        glm::mat4 model_inv = glm::inverse(m_sceneRenderer->getModelMatrix(true));

        glm::vec3 ro = glm::vec3(model_inv*glm::vec4(Manager::camera->getCameraPosition(), 1.0));
        if(!ray_initialized){
            float tangent = tan(Manager::camera->getFOV() * 0.5f);
            glm::vec2 ts = screenQuad::instance()->getTexSize();
            float ar = ts.x / ts.y;
            float u = (ts.x *0.5f + 0.5f)/ts.x * 2.0f -1.0f;
            float v = (ts.y * 0.5f + 0.5f)/ts.y * 2.0f -1.0f;
            ray_dir = glm::vec4(u* tangent*ar, v*tangent, -1.0, .0);
            ray_initialized = true;
        }

        glm::vec3 rd = glm::vec3(glm::normalize(model_inv * Manager::camera->getCameraPose() *ray_dir));
        glm::vec3 tMin = (-glm::vec3(.5f) - ro) / rd;
        glm::vec3 tMax = (glm::vec3(.5f) - ro) / rd;
        glm::vec3 t1 = min(tMin, tMax);
        glm::vec3 t2 = max(tMin, tMax);
        glm::vec2 res =  glm::vec2(glm::max(glm::max(t1.x, t1.y), t1.z), glm::min(glm::min(t2.x, t2.y), t2.z));

        //intersect
        if(Manager::isRayCut()){
            if(res.x<res.y){
            glm::vec3 pn = cam->getViewDirection();
            glm::vec3 pp = cam->getCameraPosition() + glm::normalize(pn)*0.5f;

            pp = glm::vec3(model_inv * glm::vec4(pp, 1.0f));
            pn = glm::vec3(model_inv * glm::vec4(pn, 1.0f));

            m_sceneRenderer->setCuttingPlane(pp, pn);}
            else{
                m_sceneRenderer->setCuttingPlane(glm::vec3(-10.0f), glm::vec3(1.0));
            }
        }else{
            Manager::volume_ar_hold = (res.x<res.y);
        }
    }
    m_sceneRenderer->onDrawScene();
}
JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    if(!Manager::param_bool[dvr::CHECK_AR_ENABLED]){
        if(camera_switch_dirty){
            m_sceneRenderer->setMVPStatus("template");
            camera_switch_dirty = false;
        }
        m_sceneRenderer->onDraw();
//        if(m_sceneRenderer->isDrawing())overlayController::instance()->onDraw();
    }
    else{
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        if(camera_switch_dirty){
            m_sceneRenderer->setMVPStatus("ARCam");
            camera_switch_dirty = false;
        }
        screenQuad::instance()->Clear();
        arController::instance()->onDraw();

        if(arInitialized){
            on_draw_native();
        }else{
            //update model mat of volume
            auto tplanes = arController::instance()->getTrackedPlanes();
            if(!tplanes.empty()){
                m_sceneRenderer->setVolumeRST(tplanes[0].rotMat, glm::vec3(0.2f), tplanes[0].centerVec);
                m_sceneRenderer->setMVPStatus("ARCam");
                arInitialized = true;
                on_draw_native();
            }
        }
        screenQuad::instance()->Draw();
//        if(m_sceneRenderer->isDrawing())overlayController::instance()->onDraw();
    }
}

JNI_METHOD(void, JNIsendData)(JNIEnv*env, jclass, jint target, jint id, jint chunk_size, jint unit_size, jbyteArray jdata){
    if(!g_VolumeTexData) return;
    jbyte *data = env->GetByteArrayElements(jdata, 0);
    GLubyte* buffer = g_VolumeTexData+n_data_offset[target];
    if(chunk_size !=0 && unit_size == CHANEL_NUM) memcpy(buffer, data, (size_t)chunk_size);
    else{
        int num = (chunk_size==0)? (g_img_h*g_img_w) : chunk_size / unit_size;
        if(target == LOAD_DCMI_ID){
            for(auto idx = 0; idx<num; idx++){
                buffer[CHANEL_NUM* idx] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 1] = GLubyte(data[2*idx+1]);
            }
        }else if(target == LOAD_MASK_ID){
            for(auto idx = 0; idx<num; idx++){
                buffer[CHANEL_NUM* idx + 2] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 3] = GLubyte(data[2*idx+1]);
            }
        }
    }
    n_data_offset[target] += CHANEL_NUM / unit_size * chunk_size;
    env->ReleaseByteArrayElements(jdata, data, 0);
}

JNI_METHOD(void, JNIsendDataFloats)(JNIEnv* env, jclass, jint target, jint chunk_size, jfloatArray arr){
    LOGE("====CHUNK SIZE: %d", chunk_size);
    chunk_size = 12001;
    if(target!=LOAD_CENTERLINE_ID) return;
    jfloat *data = env->GetFloatArrayElements(arr,0);
    float* cdata = new float[chunk_size-1];
    memcpy(cdata, &data[1], (chunk_size-1)* sizeof(float));
    centerline_map[(int)data[0]] = cdata;
    env->ReleaseFloatArrayElements(arr,data,0);
}

JNI_METHOD(void, JNIsendDataPrepareNative)(JNIEnv*, jclass, jint height, jint width, jint dims,jfloat sh,jfloat sw, jfloat sd, jboolean b_mask){
    CHANEL_NUM = b_mask? 4:2;
    g_img_h = height; g_img_w = width; g_img_d = dims;
    g_ssize = CHANEL_NUM * width * height;
    g_vol_len = g_ssize* dims;
    g_vol_h=sh; g_vol_w=sw; g_vol_depth=sd;
    if(g_VolumeTexData!= nullptr){delete[]g_VolumeTexData; g_VolumeTexData = nullptr;}
    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));
}

JNI_METHOD(void, JNIsendDataDone)(JNIEnv*, jclass){
    for(int i=0; i<3; i++){
        if(n_data_offset[i] != 0){
            m_sceneRenderer->assembleTexture(i, g_img_h, g_img_w, g_img_d, g_vol_h, g_vol_w, g_vol_depth, g_VolumeTexData, CHANEL_NUM);
            n_data_offset[i] = 0;
            break;
        }
    }
    if(!centerline_map.empty()){
        for(auto inst:centerline_map)
            m_sceneRenderer->setupCenterLine(inst.first, inst.second);
        centerline_map.clear();
    }
}

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass){
    if(return_pos >= g_vol_len) {
        return_pos = 0;
        return nullptr;
    }

    int rsize = fmin(1024, g_vol_len-return_pos);

    jbyteArray garr = env->NewByteArray(rsize);
    env->SetByteArrayRegion(garr,0, rsize, reinterpret_cast<jbyte*>(g_VolumeTexData + return_pos));
    return_pos+=rsize;
    return garr;
}
JNI_METHOD(void, JNIreleaseBuffer)(JNIEnv*, jclass){
    delete[]g_VolumeTexData; g_VolumeTexData = nullptr;
}
JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}

jclass FindClass(const char *classname) {
    JNIEnv *env = GetJniEnv();
    return env->FindClass(classname);
}
#include <Utils/assetLoader.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include <vector>

#include <platforms/android/ARHelpers/arController.h>
#include <vrController.h>
#include <dicomRenderer/screenQuad.h>
#include <Utils/assetLoader.h>

using namespace dvr;
namespace {
    bool arInitialized = false;
    const int LOAD_DCMI_ID = 0, LOAD_MASK_ID = 1;
    int CHANEL_NUM = 4;
    //globally
    GLubyte* g_VolumeTexData = nullptr;
    int g_img_h=0, g_img_w=0, g_img_d=0;
    size_t g_ssize = 0, g_vol_len;
    size_t n_data_offset[2] = {0};

    void setupShaderContents(){
        vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
        const char* shader_file_names[14] = {
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
                "shaders/colorViz.frag",
                "shaders/opaViz.vert",
                "shaders/opaViz.frag"
        };
        const char* android_shader_file_names[6] = {
                "shaders/arcore_screen_quad.vert",
                "shaders/arcore_screen_quad.frag",
                "shaders/pointcloud.vert",
                "shaders/pointcloud.frag",
                "shaders/plane.vert",
                "shaders/plane.frag"
        };
        for(int i = 0; i<int(dvr::SHADER_END); i++){
            std::string content;
            assetLoader::instance()->LoadTextFileFromAssetManager(shader_file_names[i], &content);
            vrc->setShaderContents(SHADER_FILES (i), content);
        }

        for(int i=0; i<int(dvr::SHADER_ANDROID_END) - int(dvr::SHADER_END); i++){
            std::string content;
            assetLoader::instance()->LoadTextFileFromAssetManager(android_shader_file_names[i], &content);
            vrController::shader_contents[dvr::SHADER_END + i] = content;
        }
    }
}

jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    new assetLoader(AAssetManager_fromJava(env, asset_manager));

    nativeAddr =  getNativeClassAddr(new vrController());
    vrController::camera = &virtualCam;
    setupShaderContents();
    return nativeAddr;
}

JNI_METHOD(void, JNIonPause)(JNIEnv* env, jclass){
    nativeApp(nativeAddr)->onPause();
    arController::instance()->onPause();
}

JNI_METHOD(void, JNIonDestroy)(JNIEnv* env, jclass){
    nativeApp(nativeAddr)->onDestroy();
    arController::instance()->onDestroy();

    delete nativeApp(nativeAddr);
    delete arController::instance();

    nativeAddr = 0;
}

JNI_METHOD(void, JNIonResume)(JNIEnv* env, jclass, jobject context, jobject activity){
    nativeApp(nativeAddr)->onResume(env, context, activity);
    arController::instance()->onResume(env, context, activity);
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
    nativeApp(nativeAddr)->onViewCreated();
    arController::instance()->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint rot, jint w, jint h){
    nativeApp(nativeAddr)->onViewChange(rot, w, h);
    arController::instance()->onViewChange(rot,w,h);
    screenQuad::instance()->onScreenSizeChange(w, h);
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //order matters
    screenQuad::instance()->Clear();

    if(vrController::param_bool[dvr::CHECK_ARENABLED]){
        arController::instance()->onDraw();
        if(!arInitialized){
            //update model mat of volume
            auto tplanes = arController::instance()->getTrackedPlanes();
            if(!tplanes.empty()){
                vrController::instance()->setVolumeRST(tplanes[0].rotMat, glm::vec3(0.2f), tplanes[0].centerVec);
                arInitialized = true;
                nativeApp(nativeAddr)->onDraw();
            }
        }else{nativeApp(nativeAddr)->onDraw();}
    }else{
        nativeApp(nativeAddr)->onDraw();
    }

    screenQuad::instance()->Draw();
    vrController::instance()->onDrawOverlays();
}

JNI_METHOD(void, JNIsendData)(JNIEnv*env, jclass, jint target, jint id, jint chunk_size, jint unit_size, jbyteArray jdata){
    //check initialization
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
        }else{
            for(auto idx = 0; idx<num; idx++){
                buffer[CHANEL_NUM* idx + 2] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 3] = GLubyte(data[2*idx+1]);
            }
        }
    }
    n_data_offset[target] += CHANEL_NUM / unit_size * chunk_size;
    env->ReleaseByteArrayElements(jdata, data, 0);
}

JNI_METHOD(void, JNIsendDataPrepare)(JNIEnv*, jclass, jint width, jint height, jint dims, jboolean b_wmask){
    CHANEL_NUM = b_wmask? 4:2;
    g_img_h = height; g_img_w = width; g_img_d = dims;
    g_ssize = CHANEL_NUM * width * height;
    g_vol_len = g_ssize* dims;

    if(g_VolumeTexData!= nullptr){delete[]g_VolumeTexData; g_VolumeTexData = nullptr;}
    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));
}

JNI_METHOD(void, JNIsendDataDone)(JNIEnv*, jclass){
    if(n_data_offset[LOAD_DCMI_ID] == 0 && n_data_offset[LOAD_MASK_ID] == 0) return;
    vrController::instance()->assembleTexture(g_img_w, g_img_h, g_img_d, g_VolumeTexData, CHANEL_NUM);
    //todo:!!attention, is the data change or not? delete here to save memory
    delete[]g_VolumeTexData; g_VolumeTexData = nullptr;
    n_data_offset[LOAD_DCMI_ID] = 0; n_data_offset[LOAD_MASK_ID] = 0;
}

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass){
    jbyteArray gdata = env->NewByteArray(g_vol_len);
    env->SetByteArrayRegion(gdata,0,g_vol_len, reinterpret_cast<jbyte*>(g_VolumeTexData));
    return gdata;
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
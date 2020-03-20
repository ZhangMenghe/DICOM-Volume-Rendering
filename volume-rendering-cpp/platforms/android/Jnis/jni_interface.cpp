#include <Utils/assetLoader.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include <vector>

#include <platforms/android/ARHelpers/arController.h>
#include <vrController.h>
#include <dicomRenderer/screenQuad.h>

using namespace dvr;
namespace {

    const int LOAD_DCMI_ID = 0, LOAD_MASK_ID = 1;
    int CHANEL_NUM = 4;
    //globally
    GLubyte* g_VolumeTexData = nullptr;
    int g_img_h, g_img_w, g_vol_dim;
    size_t g_ssize_schanel, g_ssize = 0, g_vol_len;
    size_t n_data_offset[2] = {0};
    AAssetManager * _asset_manager;

    bool ar_enabled = false;

    std::string LoadTextFile(const char* file_name) {
        std::string* out_file_text_string = new std::string();
        AAsset* asset =
                AAssetManager_open(_asset_manager, file_name, AASSET_MODE_STREAMING);
        if (asset == nullptr) {
            LOGE("Error opening asset %s", file_name);
            return "";
        }

        off_t file_size = AAsset_getLength(asset);
        out_file_text_string->resize(file_size);
        int ret = AAsset_read(asset, &out_file_text_string->front(), file_size);

        if (ret <= 0) {
            LOGE("Failed to open file: %s", file_name);
            AAsset_close(asset);
            return "";
        }
        AAsset_close(asset);
        return *out_file_text_string;
    }
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
        const char* android_shader_file_names[2] = {
                "shaders/arcore_screen_quad.vert",
                "shaders/arcore_screen_quad.frag"
        };
        for(int i = 0; i<int(dvr::SHADER_END); i++)
            vrc->setShaderContents(SHADER_FILES (i), LoadTextFile(shader_file_names[i]));
        for(int i=0; i<int(dvr::SHADER_ANDROID_END) - int(dvr::SHADER_END); i++)
            vrController::shader_contents[dvr::SHADER_END + i] = LoadTextFile(android_shader_file_names[i]);
//            vrc->setShaderContents(ANDROID_SHADER_FILES(i), LoadTextFile(android_shader_file_names[i]));
    }
}
void JNIEnableDisableAR(){
    ar_enabled = !ar_enabled;
}
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    _asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAddr =  getNativeClassAddr(new vrController());
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
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    arController::instance()->onDraw();
    nativeApp(nativeAddr)->onDraw();
    screenQuad::instance()->Draw();
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
}

JNI_METHOD(void, JNIsetupDCMIConfig)(JNIEnv*, jclass, jint width, jint height, jint dims, jboolean b_wmask){
    CHANEL_NUM = b_wmask? 4:2;
    g_img_h = height; g_img_w = width;
    g_ssize_schanel = width * height;
    g_vol_dim = dims;
    g_ssize = CHANEL_NUM * g_ssize_schanel;
    g_vol_len = g_ssize* dims;

    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));

    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->setVolumeConfig(width, height, dims);
}

JNI_METHOD(void, JNIAssembleVolume)(JNIEnv*, jclass){
    if(n_data_offset[LOAD_DCMI_ID] == 0 && n_data_offset[LOAD_MASK_ID] == 0) return;
    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->assembleTexture(g_VolumeTexData, CHANEL_NUM);
    n_data_offset[LOAD_DCMI_ID] = 0; n_data_offset[LOAD_MASK_ID] = 0;
}

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass){
    jbyteArray gdata = env->NewByteArray(g_vol_len);
    env->SetByteArrayRegion(gdata,0,g_vol_len, reinterpret_cast<jbyte*>(g_VolumeTexData));
    return gdata;
}
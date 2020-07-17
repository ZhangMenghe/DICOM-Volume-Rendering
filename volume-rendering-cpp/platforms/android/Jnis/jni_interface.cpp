#include <assetLoader.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include "vrController.h"

#include <android/bitmap.h>
#include <vector>
#include <overlayController.h>

using namespace dvr;
namespace {
    const int LOAD_DCMI_ID = 0, LOAD_MASK_ID = 1;
    int CHANEL_NUM = 4;
    //globally
    GLubyte* g_VolumeTexData = nullptr;
    int g_img_h=0, g_img_w=0, g_img_d=0;
    float g_vol_h, g_vol_w, g_vol_depth = 0;
    size_t g_ssize = 0, g_vol_len;
    size_t n_data_offset[3] = {0};
    AAssetManager * _asset_manager;
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
        for(int i = 0; i<int(dvr::SHADER_END); i++)
            vrc->setShaderContents(SHADER_FILES (i), LoadTextFile(shader_file_names[i]));
    }
}
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    _asset_manager = AAssetManager_fromJava(env, asset_manager);
    //order matters
    manager = new Manager;
    nativeAddr =  getNativeClassAddr(new vrController());
    setupShaderContents();
    return nativeAddr;
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
    nativeApp(nativeAddr)->onViewCreated();
    overlayController::instance()->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint w, jint h){
    manager->onViewChange(w, h);
    nativeApp(nativeAddr)->onViewChange(w, h);
    overlayController::instance()->onViewChange(w, h);
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    nativeApp(nativeAddr)->onDraw();
    if(vrController::instance()->isDrawing()) overlayController::instance()->onDraw();
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
            vrController::instance()->assembleTexture(i, g_img_h, g_img_w, g_img_d, g_vol_h, g_vol_w, g_vol_depth, g_VolumeTexData, CHANEL_NUM);
            n_data_offset[i] = 0;
            break;
        }
    }
}

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass){
    jbyteArray garr= env->NewByteArray(g_vol_len);
    env->SetByteArrayRegion(garr,0,g_vol_len, reinterpret_cast<jbyte*>(g_VolumeTexData));
    return garr;
}
JNI_METHOD(void, JNIreleaseBuffer)(JNIEnv*, jclass){
    delete[]g_VolumeTexData; g_VolumeTexData = nullptr;
}
#include <assetLoader.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include "vrController.h"

#include <android/bitmap.h>
#include <vector>

using namespace dvr;
namespace {
    const int LOAD_DCMI_ID = 0, LOAD_MASK_ID = 1;
    const int CHANEL_NUM = 4;
//    CHANEL_MASK=2;

    //globally
    GLubyte* g_VolumeTexData = nullptr;
//    GLubyte* g_VolumeMaskData = nullptr;
    int g_img_h, g_img_w, g_vol_dim;
    size_t g_ssize_schanel, g_ssize = 0, g_vol_len;
//    g_mask_ssize, g_mask_len;
    size_t n_data_offset[2] = {0};
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
    nativeAddr =  getNativeClassAddr(new vrController());
    setupShaderContents();
    return nativeAddr;
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
    nativeApp(nativeAddr)->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint w, jint h){
    nativeApp(nativeAddr)->onViewChange(w, h);
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    nativeApp(nativeAddr)->onDraw();
}

JNI_METHOD(void, JNIsendData)(JNIEnv*env, jclass, jint target, jint id, jint chunk_size, jint unit_size, jbyteArray jdata){
    //check initialization
    if(!g_VolumeTexData) return;
    jbyte *data = env->GetByteArrayElements(jdata, 0);

    GLubyte* buffer = g_VolumeTexData+n_data_offset[target];
    if(chunk_size !=0 && unit_size == 4) memcpy(buffer, data, (size_t)chunk_size);
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
    n_data_offset[target] += (chunk_size==0)?g_ssize:(CHANEL_NUM / unit_size * chunk_size);
}

JNI_METHOD(void, JNIsendDCMImg)(JNIEnv* env, jclass, jint id, jint chunk_size, jbyteArray data){
    //check initialization
    if(!g_VolumeTexData) return;
    jbyte *c_array = env->GetByteArrayElements(data, 0);
    GLubyte* buffer = g_VolumeTexData+n_data_offset[LOAD_DCMI_ID];

    if(chunk_size!=0) memcpy(buffer, c_array, chunk_size);
    else{
        int x, y, idx = 0;
        for (y = 0; y < g_img_h; y++) {
            for (x = 0; x < g_img_w; x++) {
                buffer[CHANEL_NUM* idx] = GLubyte(c_array[2*idx]);
                buffer[CHANEL_NUM* idx + 1] = GLubyte(c_array[2*idx+1]);
                idx++;
            }
        }
    }
    n_data_offset[LOAD_DCMI_ID]+=(chunk_size==0)?g_ssize:chunk_size;
}

JNI_METHOD(void, JNIsendDCMIMask)(JNIEnv* env, jclass, jint id,  jint chunk_size, jbyteArray data){
    //todo
    //    if(!g_VolumeMaskData) g_VolumeMaskData = new GLubyte[g_mask_len];
//
//    jbyte *c_array = env->GetByteArrayElements(data, 0);
//    auto* buffer = g_VolumeMaskData+n_data_offset[LOAD_MASK_ID];
//
//    auto single_size = (chunk_size==0)?g_mask_ssize:chunk_size;
//    memcpy(buffer, c_array, single_size);
//    n_data_offset[LOAD_MASK_ID]+=single_size;
}

JNI_METHOD(void, JNIsetupDCMIConfig)(JNIEnv*, jclass, jint width, jint height, jint dims){
    g_img_h = height; g_img_w = width;
    g_ssize_schanel = width * height;
    g_vol_dim = dims;
    g_ssize = CHANEL_NUM * g_ssize_schanel;
//    g_mask_ssize = g_ssize_schanel * CHANEL_MASK;

    g_vol_len = g_ssize* dims;
//    g_mask_len = g_mask_ssize*dims;

    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));

    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->setVolumeConfig(width, height, dims);
}

JNI_METHOD(void, JNIAssembleMask)(JNIEnv*, jclass){
    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->updateTexture(g_VolumeTexData);
    n_data_offset[LOAD_MASK_ID] = 0;
}

JNI_METHOD(void, JNIAssembleVolume)(JNIEnv*env, jclass){
    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->assembleTexture(g_VolumeTexData);
    n_data_offset[LOAD_DCMI_ID] = 0;
}

JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass, jboolean b_getmask){
    jbyteArray gdata;
//    if(b_getmask){
//        gdata = env->NewByteArray(g_mask_len);
//        env->SetByteArrayRegion(gdata, 0, g_mask_len, reinterpret_cast<jbyte*>(g_VolumeMaskData));
//    }else{
        gdata = env->NewByteArray(g_vol_len);
        env->SetByteArrayRegion(gdata,0,g_vol_len, reinterpret_cast<jbyte*>(g_VolumeTexData));
//    }
    return gdata;
}
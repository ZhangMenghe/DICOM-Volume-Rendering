#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include "jni_interface.h"
#include "vrController.h"

#include <android/bitmap.h>
#include <AndroidUtils/AndroidHelper.h>
#include <vector>

using namespace dvr;
namespace {
    typedef struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } argb;
    class dcmImage{
    public:
        GLubyte * data;
        float location;
        dcmImage(GLubyte * _data, float _location):
                data(_data), location(_location){}

    };
    std::vector<dcmImage *> images_;
    int img_height, img_width;
    const int CHANEL_NUM = 4;

    //globally
    GLubyte* g_VolumeTexData = nullptr;
    uint16_t* g_VolumeMaskData = nullptr;
    int g_img_h, g_img_w, g_vol_dim;
    size_t g_ssize_schanel, g_ssize = 0, g_vol_len;
    bool b_pre_load = false;
}
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAddr =  getNativeClassAddr(new vrController(cpp_asset_manager));
    return nativeAddr;
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
//    size_t dimensions = images_.size();
//
//    size_t data_size = CHANEL_NUM * img_width * img_height * dimensions;
//    auto *data = new GLubyte[data_size];
//    auto each_size = CHANEL_NUM * img_height * img_width* sizeof(GLubyte);
//    for(int i=0; i<dimensions; i++)
//        memcpy(data+i*each_size, images_[i]->data, each_size);
//
//    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
//
//    vrc->assembleTexture(data, img_width, img_height, dimensions, CHANEL_NUM);
//    delete[]data;
    if(b_pre_load){
        vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
        vrc->assembleTexture(g_VolumeTexData);
    }
    nativeApp(nativeAddr)->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint w, jint h){
    nativeApp(nativeAddr)->onViewChange(w, h);
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jclass){
    nativeApp(nativeAddr)->onDraw();
}

void load_mask_from_bitmap(JNIEnv* env, jobject bitmap, GLubyte*& data, int w, int h ){
    AndroidBitmapInfo srcInfo;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &srcInfo)) {
        LOGE("====get bitmap info failed");
        return;
    }
    void * buffer;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &buffer)) {
        LOGE("====lock src bitmap failed");
        return;
    }

    int x, y, idx = 0;
    for (y = 0; y < h; y++) {
        argb * line = (argb *) buffer;
        for (x = 0; x < w; x++) {
            data[CHANEL_NUM*idx+1] = line[x].red;
            idx++;
        }
        buffer = (char *) buffer + srcInfo.stride;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}

void convert_bitmap(JNIEnv* env, jobject bitmap, GLubyte*& data, int&w, int &h, int offset ){
    AndroidBitmapInfo srcInfo;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &srcInfo)) {
        LOGE("====get bitmap info failed");
        return;
    }
    void * buffer;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &buffer)) {
        LOGE("===lock src bitmap failed");
        return;
    }
    if(!data){
        LOGI("width=%d; height=%d; stride=%d; format=%d;flag=%d",
             srcInfo.width, //  width=2700 (900*3)
             srcInfo.height, // height=2025 (675*3)
             srcInfo.stride, // stride=10800 (2700*4)
             srcInfo.format, // format=1 (ANDROID_BITMAP_FORMAT_RGBA_8888=1)
             srcInfo.flags); // flags=0 (ANDROID_BITMAP_RESULT_SUCCESS=0)
        w = srcInfo.width; h = srcInfo.height;

        size_t size = srcInfo.width * srcInfo.height;

        data = new GLubyte[CHANEL_NUM*size];
        memset(data, 0x00, CHANEL_NUM * size * sizeof(GLubyte));
    }

    int x, y, idx = 0;
    for (y = 0; y < h; y++) {
        argb * line = (argb *) buffer;
        for (x = 0; x < w; x++) {
            data[CHANEL_NUM*idx + offset] = GLubyte(line[x].red);
            idx++;
        }

        buffer = (char *) buffer + srcInfo.stride;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}
JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv* env, jclass , jobjectArray img_arr, jobjectArray msk_arr, jint size){
    //get dcmImg class defined in java
    jclass imgClass = env->FindClass("helmsley/vr/Utils/dcmImage");
    jobject img, bitmap, bitmap_mask;
    jfieldID bitmap_id, bm_mask_id, location_id, thickness_id;
    float location, thickness;
    int valid_num = 0;
    int width, height;
    for(int i=0; i<size; i++) {
        img = env->GetObjectArrayElement(img_arr, i);

        thickness_id = env->GetFieldID(imgClass, "thickness", "F");
        thickness = env->GetFloatField(img, thickness_id);
        if(thickness == -1)//invalid
            continue;
        else{valid_num++;}

        location_id = env->GetFieldID(imgClass, "location", "F");
        location = env->GetFloatField(img, location_id);

        bitmap_id = env->GetFieldID(imgClass, "bitmap", "Landroid/graphics/Bitmap;");
        bitmap = env->GetObjectField(img, bitmap_id);

        GLubyte * data = nullptr;
        convert_bitmap(env, bitmap, data, width, height, 0);
        img_height = height; img_width = width;
        images_.push_back(new dcmImage(
                data,
                location));
    }
    std::sort(images_.begin(), images_.end(),
              [](const dcmImage* img1, const dcmImage* img2){return img1->location < img2->location;});
    if(env->GetArrayLength(msk_arr)){
        for(int idx = 0; idx<size; idx++){
            bitmap_mask = env->GetObjectArrayElement(msk_arr, idx);
            convert_bitmap(env, bitmap_mask, images_[idx]->data, width, height, 1);
        }
    }
}

JNI_METHOD(void, JNIsendDCMImg)(JNIEnv* env, jclass, jint id, jfloat pos, jbyteArray data){
    if(!g_VolumeTexData) return; //check initialization
    jbyte *c_array = env->GetByteArrayElements(data, 0);
    if(id == -1){
        memcpy(g_VolumeTexData, c_array, g_vol_len*sizeof(GLubyte));
        b_pre_load = true;
        return;
    }

    GLubyte* buffer = g_VolumeTexData+id*g_ssize;
    int x, y, idx = 0;
    for (y = 0; y < g_img_h; y++) {
        for (x = 0; x < g_img_w; x++) {
            buffer[CHANEL_NUM* idx] = GLubyte(c_array[2*idx]);
            buffer[CHANEL_NUM* idx + 1] = GLubyte(c_array[2*idx+1]);
            idx++;
        }
    }
}
JNI_METHOD(void, JNIsendDCMIMask)(JNIEnv* env, jclass, jint id,  jfloat pos, jbyteArray data){
    auto g_mask_size = g_img_w * g_img_h * g_vol_dim;
    if(!g_VolumeMaskData) g_VolumeMaskData = new uint16_t[g_mask_size];

    jbyte *c_array = env->GetByteArrayElements(data, 0);

    if(id == -1){
        memcpy(g_VolumeMaskData, c_array, g_mask_size*sizeof(uint16_t));
    }else{
        uint16_t* buffer = g_VolumeMaskData+id*g_ssize_schanel;
        for(int i=0 ;i<g_ssize_schanel; i++){
            buffer[i] = uint16_t ((((uint16_t)c_array[2*i + 1])<<8)+c_array[2*i]);
        }
    }

    if(id == -1 || id == (g_vol_dim - 1)){
        vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
        vrc->assembleTextureMask(g_VolumeMaskData);
    }
}

JNI_METHOD(void, JNIsetupDCMIConfig)(JNIEnv*, jclass, jint width, jint height, jint dims){
    g_img_h = height; g_img_w = width;
    g_ssize_schanel = width * height;
    g_vol_dim = dims;
    g_ssize = CHANEL_NUM * g_ssize_schanel;
    g_vol_len = g_ssize* dims;
    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));

    //todo: send dim to native
    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->setVolumeConfig(width, height, dims);
}
JNI_METHOD(void, JNIAssembleVolume)(JNIEnv*, jclass){
    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));
    vrc->assembleTexture(g_VolumeTexData);
}
JNI_METHOD(jbyteArray, JNIgetVolumeData)(JNIEnv* env, jclass, jboolean b_getmask){
    if(b_getmask){
        auto msize = g_ssize_schanel * g_vol_dim;
        jbyteArray gdata = env->NewByteArray(msize);
        env->SetByteArrayRegion(gdata,0,msize, reinterpret_cast<jbyte*>(g_VolumeMaskData));
        return gdata;
    }
    jbyteArray gdata = env->NewByteArray(g_vol_len);
    env->SetByteArrayRegion(gdata,0,g_vol_len, reinterpret_cast<jbyte*>(g_VolumeTexData));
    return gdata;
}
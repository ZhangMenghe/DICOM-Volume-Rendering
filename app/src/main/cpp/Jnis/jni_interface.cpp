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
}
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAddr =  getNativeClassAddr(new vrController(cpp_asset_manager));
    return nativeAddr;
}

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass){
    size_t dimensions = images_.size();

    size_t data_size = CHANEL_NUM * img_width * img_height * dimensions;
    auto *data = new GLubyte[data_size];
    auto each_size = CHANEL_NUM * img_height * img_width* sizeof(GLubyte);
    for(int i=0; i<dimensions; i++)
        memcpy(data+i*each_size, images_[i]->data, each_size);

    vrController* vrc = dynamic_cast<vrController*>(nativeApp(nativeAddr));

    vrc->assembleTexture(data, img_width, img_height, dimensions, CHANEL_NUM);
    delete[]data;
    nativeApp(nativeAddr)->onViewCreated();
}

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint w, jint h){
    nativeApp(nativeAddr)->onViewChange(w, h);
}

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jobject){
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
JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv* env, jobject, jobjectArray img_arr, jobjectArray msk_arr, jint size){
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
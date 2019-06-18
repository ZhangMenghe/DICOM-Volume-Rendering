#ifndef ANDROID_HELPER
#define ANDROID_HELPER

#include <string>
#include <map>
#include <android/asset_manager.h>

#include <stack>
#include <iosfwd>
#include <GLES3/gl32.h>

#include <android/log.h>
#define TAG "ANDROID-HELPER"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

class assetLoader{
private:
    static assetLoader* _myPtr;
    AAssetManager * const _asset_manager;
public:
    static assetLoader * instance();
    assetLoader(AAssetManager * assetManager);
    bool LoadTextFileFromAssetManager(const char* file_name, std::string* out_file_text_string);
};



#endif

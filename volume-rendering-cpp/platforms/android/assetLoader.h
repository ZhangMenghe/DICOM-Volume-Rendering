#ifndef PLATFORM_ANDROID_ASSETLOADER_H
#define PLATFORM_ANDROID_ASSETLOADER_H

#include <string>
#include <android/asset_manager.h>
#include <platforms/platform.h>

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

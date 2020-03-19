#include <platforms/android/Utils/assetLoader.h>

assetLoader* assetLoader::_myPtr = nullptr;

assetLoader::assetLoader(AAssetManager * const assetManager):
_asset_manager(assetManager){
    _myPtr = this;
}
assetLoader* assetLoader::instance() {
    return _myPtr;
}

bool assetLoader::LoadTextFileFromAssetManager(const char* file_name,std::string* out_file_text_string) {
    AAsset* asset =
            AAssetManager_open(_asset_manager, file_name, AASSET_MODE_STREAMING);
    if (asset == nullptr) {
        LOGE("Error opening asset %s", file_name);
        return false;
    }

    off_t file_size = AAsset_getLength(asset);
    out_file_text_string->resize(file_size);
    int ret = AAsset_read(asset, &out_file_text_string->front(), file_size);

    if (ret <= 0) {
        LOGE("Failed to open file: %s", file_name);
        AAsset_close(asset);
        return false;
    }

    AAsset_close(asset);
    return true;
}
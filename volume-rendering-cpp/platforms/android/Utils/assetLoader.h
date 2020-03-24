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
    // Load a text file from assets folder.
    //
    // @param asset_manager, AAssetManager pointer.
    // @param file_name, path to the file, relative to the assets folder.
    // @param out_string, output string.
    // @return true if the file is loaded correctly, otherwise false.
    bool LoadTextFileFromAssetManager(const char* file_name,
                                      std::string* out_file_text_string);

    // Load png file from assets folder and then assign it to the OpenGL target.
    // This method must be called from the renderer thread since it will result in
    // OpenGL calls to assign the image to the texture target.
    //
    // @param target, openGL texture target to load the image into.
    // @param path, path to the file, relative to the assets folder.
    // @return true if png is loaded correctly, otherwise false.
    bool LoadPngFromAssetManager(int target, const std::string& path);

    // Load obj file from assets folder from the app.
    //
    // @param asset_manager, AAssetManager pointer.
    // @param file_name, name of the obj file.
    // @param out_vertices, output vertices.
    // @param out_normals, output normals.
    // @param out_uv, output texture UV coordinates.
    // @param out_indices, output triangle indices.
    // @return true if obj is loaded correctly, otherwise false.
    bool LoadObjFile(const std::string& file_name,
                     std::vector<GLfloat>* out_vertices,
                     std::vector<GLfloat>* out_normals,
                     std::vector<GLfloat>* out_uv,
                     std::vector<GLushort>* out_indices);
};

#endif

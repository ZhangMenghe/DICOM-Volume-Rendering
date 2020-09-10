#ifndef PLATFORMS_DESKTOP_UTILS_FILELOADER_H
#define PLATFORMS_DESKTOP_UTILS_FILELOADER_H
#include <string>
#include <fstream>
#include <platforms/desktop/common.h>

#include <platforms/platform.h>
#include <vrController.h>


inline std::string LoadTextFile(const char* filename){
    std::string content;
    #ifdef RESOURCE_DESKTOP_DIR
    std::ifstream ShaderStream(PATH(filename), std::ios::in);
    #else
    std::ifstream ShaderStream(filename, std::ios::in);
    #endif

       if(ShaderStream.is_open()){
           std::string Line = "";
           while(getline(ShaderStream, Line)) content += "\n" + Line;
           ShaderStream.close();
       }else{
           LOGE("====Failed to load file: %s", filename);
       }
       return content;
}
inline void setupShaderContents(vrController* vrc){
        const char* shader_file_names[18] = {
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
                "shaders/opaViz.frag",
                "shaders/marching_cube.glsl",
                "shaders/marching_cube_clear.glsl",
                "shaders/marching_cube_draw.vert",
                "shaders/marching_cube_draw.frag"
        };
        for(int i = 0; i<int(dvr::SHADER_END); i++)
            vrc->setShaderContents(dvr::SHADER_FILES (i), LoadTextFile(shader_file_names[i]));        
    }
#endif 

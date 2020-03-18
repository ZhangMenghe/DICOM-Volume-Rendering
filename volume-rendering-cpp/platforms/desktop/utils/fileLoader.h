#ifndef PLATFORMS_DESKTOP_UTILS_FILELOADER_H
#define PLATFORMS_DESKTOP_UTILS_FILELOADER_H
#include <string>
#include <fstream>
#include <platforms/desktop/common.h>

#include <platforms/platform.h>
#include <vrController.h>


inline std::string LoadTextFile(const char* filename){
    std::string content;
    std::ifstream ShaderStream(PATH(filename), std::ios::in);
       if(ShaderStream.is_open()){
           std::string Line = "";
           while(getline(ShaderStream, Line)) content += "\n" + Line;
           ShaderStream.close();
           std::cout<<PATH(filename)<<std::endl;
       }else{
           LOGE("====Failed to load file: %s", filename);
       }
       return content;
}
inline void setupShaderContents(vrController* vrc){
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
            vrc->setShaderContents(dvr::SHADER_FILES (i), LoadTextFile(shader_file_names[i]));
    }
#endif 

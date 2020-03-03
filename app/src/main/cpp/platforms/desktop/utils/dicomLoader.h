#ifndef PLATFORMS_DESKTOP_UTILS_DICOMLOADER_H
#define PLATFORMS_DESKTOP_UTILS_DICOMLOADER_H

#include <cstddef>
#include <string>
#include <fstream>
#include <platforms/platform.h>

inline std::string loadTextFile(const char* filename){
    std::string content;
    std::ifstream ShaderStream(PATH(filename), std::ios::in);
    if(ShaderStream.is_open()){
        std::string Line = "";
        while(getline(ShaderStream, Line))
            content += "\n" + Line;
            ShaderStream.close();
    }else{
        LOGE("====Failed to load file: %s", filename);
    }
    return content; 
}
class dicomLoader{
public:
    void setupDCMIConfig(int width, int height, int dims);
    bool loadDicomFiles(std::string filename);
    GLubyte* getVolumeData(){return g_VolumeTexData;}
    void reset(){
        delete[] g_VolumeTexData;
        g_VolumeTexData = nullptr;
        n_data_offset = 0;
    }
private:
    int img_w, img_h, vol_dim;
    size_t single_csize, single_size, total_size;
    size_t mask_single_csize, mask_total_size;
    const int CHANEL_NUM = 4, CHANEL_MASK = 2;
    size_t n_data_offset = 0, n_mask_offset=0;

    GLubyte* g_VolumeTexData = nullptr;
    void send_dicom_data(int id, int chunk_size, char* data);
};

#endif
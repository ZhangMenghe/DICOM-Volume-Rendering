#ifndef PLATFORMS_DESKTOP_UTILS_DICOMLOADER_H
#define PLATFORMS_DESKTOP_UTILS_DICOMLOADER_H

#include <cstddef>
#include <string>
#include <fstream>
#include <platforms/platform.h>

typedef enum{
    LOAD_DICOM = 0,
    LOAD_MASK
}mLoadTarget;

class dicomLoader{
public:
    void setupDCMIConfig(int width, int height, int dims, bool b_wmask);
    bool loadData(std::string filename, mLoadTarget target, int unit_size=2);
    bool loadData(std::string dicom_path, std::string mask_path,int data_unit_size=2, int mask_unit_size=2);
    int getChannelNum(){return CHANEL_NUM;}
    GLubyte* getVolumeData(){return g_VolumeTexData;}
    void reset(){
        delete[] g_VolumeTexData;
        g_VolumeTexData = nullptr;
        for(auto& offset:n_data_offset) offset = 0;
    }
private:
    int CHANEL_NUM = 4;
    GLubyte* g_VolumeTexData = nullptr;
    int g_img_h=0, g_img_w=0, g_img_d=0;
    float g_vol_h, g_vol_w, g_vol_depth = 0;
    size_t g_ssize = 0, g_vol_len;
    size_t n_data_offset[3] = {0};

    void send_dicom_data(mLoadTarget target, int id, int chunk_size, int unit_size, char* data);
};

#endif
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
        n_data_offset = 0;
    }
private:
    int img_w, img_h, vol_dim;
    size_t single_csize, single_size, total_size;
    size_t mask_single_csize, mask_total_size;
    int CHANEL_NUM = 4;
    size_t n_data_offset = 0, n_mask_offset=0;

    GLubyte* g_VolumeTexData = nullptr;
    void send_dicom_data(mLoadTarget target, int id, int chunk_size, int unit_size, char* data);
};

#endif
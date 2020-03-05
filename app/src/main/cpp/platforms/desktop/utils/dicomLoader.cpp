#include "dicomLoader.h"
#include <platforms/desktop/common.h>
#include <cstring> //memset

void dicomLoader::setupDCMIConfig(int width, int height, int dims){
    img_w = width; img_h=height; vol_dim = dims;
    single_csize = img_h * img_w;
    single_size = single_csize * CHANEL_NUM;
    total_size = single_size * vol_dim;
    
	g_VolumeTexData = new GLubyte[total_size];
    memset(g_VolumeTexData, 0x00, total_size * sizeof(GLubyte));
}
bool dicomLoader::loadData(std::string dicom_path, std::string mask_path, int data_unit_size, int mask_unit_size){
    loadData(dicom_path, LOAD_DICOM, data_unit_size);
    loadData(mask_path, LOAD_MASK, mask_unit_size);
}

bool dicomLoader::loadData(std::string filename, mLoadTarget target, int unit_size){
    char buffer[1024];
    std::ifstream inFile (PATH(filename), std::ios::in | std::ios::binary);
    if(!inFile.is_open()) return false;

    for(int id = 0; !inFile.eof(); id++){
        inFile.read(buffer, 1024);
        std::streamsize len = inFile.gcount();
        if(len == 0) continue;
        send_dicom_data(target, id, len, unit_size, buffer);
    }
    n_data_offset = 0;
    return true;
}

void dicomLoader::send_dicom_data(mLoadTarget target, int id, int chunk_size, int unit_size, char* data){
    //check initialization
    if(!g_VolumeTexData) return;
    GLubyte* buffer = g_VolumeTexData+n_data_offset;
    if(chunk_size !=0 && unit_size == 4) memcpy(buffer, data, chunk_size);
    else{
        int num = (chunk_size==0)? (img_h*img_w) : chunk_size / unit_size;
        if(target == LOAD_DICOM){
            for(auto idx = 0; idx<num; idx++){
                buffer[CHANEL_NUM* idx] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 1] = GLubyte(data[2*idx+1]);
            }
        }else{
            for(auto idx = 0; idx<num; idx++){
                buffer[CHANEL_NUM* idx + 2] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 3] = GLubyte(data[2*idx+1]);
            }
        }
    }
    n_data_offset += (chunk_size==0)?single_size:(CHANEL_NUM / unit_size * chunk_size);   
}

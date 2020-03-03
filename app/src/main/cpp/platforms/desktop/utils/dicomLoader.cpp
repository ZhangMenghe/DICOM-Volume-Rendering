#include "dicomLoader.h"
#include <cstring> //memset

void dicomLoader::setupDCMIConfig(int width, int height, int dims){
    img_w = width; img_h=height; vol_dim = dims;
    single_csize = img_h * img_w;
    single_size = single_csize * CHANEL_NUM;
    total_size = single_size * vol_dim;
    
	g_VolumeTexData = new GLubyte[total_size];
    memset(g_VolumeTexData, 0x00, total_size * sizeof(GLubyte));
}

bool dicomLoader::loadDicomFiles(std::string filename){
    char buffer[1024];
    std::ifstream inFile (PATH(filename), std::ios::in | std::ios::binary);
    if(!inFile.is_open()) return false;

    for(int id = 0; !inFile.eof(); id++){
        inFile.read(buffer, 1024);
        std::streamsize len = inFile.gcount();
        if(len == 0) continue;
        send_dicom_data(id, len, buffer);
    }
    return true;
}

void dicomLoader::send_dicom_data(int id, int chunk_size, char* data){
    //check initialization
    if(!g_VolumeTexData) return;
    GLubyte* buffer = g_VolumeTexData+n_data_offset;

    if(chunk_size!=0) memcpy(buffer, data, chunk_size);
    else{
        int x, y, idx = 0;
        for (y = 0; y < img_h; y++) {
            for (x = 0; x < img_w; x++) {
                buffer[CHANEL_NUM* idx] = GLubyte(data[2*idx]);
                buffer[CHANEL_NUM* idx + 1] = GLubyte(data[2*idx+1]);
                idx++;
            }
        }
    }
    n_data_offset+=(chunk_size==0)?single_size:chunk_size;   
}

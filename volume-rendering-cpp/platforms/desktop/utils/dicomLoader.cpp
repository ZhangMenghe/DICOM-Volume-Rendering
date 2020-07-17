#include "dicomLoader.h"
#include <platforms/desktop/common.h>
#include <cstring> //memset

void dicomLoader:: setupDCMIConfig(int height, int width, int dims, float sh, float sw, float sd, bool b_wmask){
    CHANEL_NUM = b_wmask? 4:2;
    g_img_h = height; g_img_w = width; g_img_d = dims;
    g_ssize = CHANEL_NUM * width * height;
    g_vol_len = g_ssize* dims;
    g_vol_h=sh; g_vol_w=sw; g_vol_depth=sd;
    if(g_VolumeTexData!= nullptr){delete[]g_VolumeTexData; g_VolumeTexData = nullptr;}
    g_VolumeTexData = new GLubyte[ g_vol_len];
    memset(g_VolumeTexData, 0x00, g_vol_len * sizeof(GLubyte));
}
bool dicomLoader::loadData(std::string dicom_path, std::string mask_path, int data_unit_size, int mask_unit_size){
    return (loadData(dicom_path, LOAD_DICOM, data_unit_size)
    && loadData(mask_path, LOAD_MASK, mask_unit_size));
}

bool dicomLoader::loadData(std::string filename, mLoadTarget target, int unit_size){
    char buffer[1024];
    #ifdef RESOURCE_DESKTOP_DIR
    std::ifstream inFile (PATH(filename), std::ios::in | std::ios::binary);
    #else
    std::ifstream inFile (filename, std::ios::in | std::ios::binary);
    #endif

    if(!inFile.is_open()) return false;

    for(int id = 0; !inFile.eof(); id++){
        inFile.read(buffer, 1024);
        std::streamsize len = inFile.gcount();
        if(len == 0) continue;
        send_dicom_data(target, id, len, unit_size, buffer);
    }
    n_data_offset[(int)target] = 0;
    return true;
}

void dicomLoader::send_dicom_data(mLoadTarget target, int id, int chunk_size, int unit_size, char* data){
    //check initialization
    if(!g_VolumeTexData) return;
    GLubyte* buffer = g_VolumeTexData+n_data_offset[(int)target];
    if(chunk_size !=0 && unit_size == 4) memcpy(buffer, data, chunk_size);
    else{
        int num = (chunk_size==0)? (g_img_h*g_img_w) : chunk_size / unit_size;
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
   n_data_offset[target] += CHANEL_NUM / unit_size * chunk_size;   
}
void dicomLoader::startToAssemble(vrController* controller){
    std::cout<<g_img_h<<" "<<g_img_w<< " "<< g_img_d<< " "<< g_vol_h<<" "<< g_vol_w<<" "<< g_vol_depth<<std::endl;
    controller->assembleTexture(2, g_img_h, g_img_w, g_img_d, g_vol_h, g_vol_w, g_vol_depth, g_VolumeTexData, CHANEL_NUM);

	// controller->assembleTexture(2, dims[0], dims[1], dims[2],-1,-1,-1, loader_.getVolumeData(), 4);
}

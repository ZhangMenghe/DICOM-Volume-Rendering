#ifndef CLAHE_MANAGER_H
#define CLAHE_MANAGER_H

#include "ComputeCLAHE.h"
#include "Constants.h"
#include <GLPipeline/Texture.h>
class claheManager{
private:
    ComputeCLAHE* m_computer = nullptr;
    glm::uvec3 numSB_3D;
    glm::uvec3 min3D, max3D;
    float clipLimit3D;

    GLuint _3D_CLAHE;
    GLuint _FocusedCLAHE;
    GLuint _MaskedCLAHE;
    GLuint _currTexture;
    dvr::CLAHE_OPTIONS _textureMode;

    unsigned int outputGrayvals_3D = 65536;
    unsigned int inputGrayvals_3D = 65536;
    unsigned int numOrgans = 7;
    float clipStep = 0.05f;
    bool m_dirty;
    glm::uvec3 m_3d_limit;
    bool ready_to_compute = false;
public:
    claheManager();
    GLuint getCLAHETexture(dvr::CLAHE_OPTIONS mode){
        if(mode == dvr::CLAHE_3D) return _3D_CLAHE;
        if(mode == dvr::CLAHE_FOCUSED) return _FocusedCLAHE;
        return _MaskedCLAHE;
    }
    void onUpdate();
    void ApplyChanges(){ready_to_compute = true;}
    GLuint getCurrentTexture(){return _currTexture;}
    void setCurrentTexMode(dvr::CLAHE_OPTIONS mode);
    void onVolumeDataUpdate(GLuint tex_volume, GLuint tex_mask, glm::vec3 volDim);
    void onClipLimitChange(bool increase);
    void onSubBlockNumChange(bool increase);
    void onFocusRegionChange(glm::uvec3 step, bool is_scale = true, bool is_enlarge=true);
};
#endif
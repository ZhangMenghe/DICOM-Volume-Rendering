#ifndef CLAHE_MANAGER_H
#define CLAHE_MANAGER_H

#include "ComputeCLAHE.h"
#include <GLPipeline/Texture.h>

class claheManager{
private:
    enum class TextureMode {
        _CLAHE,
        _FOCUSED,
        _MASKED
    };

    ComputeCLAHE m_computer;
    glm::uvec3 numSB_3D = glm::uvec3(4, 4, 2);
    glm::uvec3 min3D = glm::uvec3(200, 200, 40);
    glm::uvec3 max3D = glm::uvec3(400, 400, 90);
    float clipLimit3D = 0.85f;

    GLuint _3D_CLAHE;
    GLuint _FocusedCLAHE;
    GLuint _MaskedCLAHE;
    GLuint _currTexture;
    TextureMode _textureMode;

    unsigned int outputGrayvals_3D = 65536;
    unsigned int inputGrayvals_3D = 65536;
    unsigned int numOrgans = 4;

    void on_focus_change();
public:
    claheManager(GLuint tex_volume, GLuint tex_mask, glm::vec3 volDim);
    GLuint getCLAHETexture(TextureMode mode){
        if(mode == TextureMode::_CLAHE) return _3D_CLAHE;
        if(mode == TextureMode::_FOCUSED) return _FocusedCLAHE;
        return _MaskedCLAHE;
    }
    GLuint getCurrentTexture(){return _currTexture;}
    void setCurrentTexMode(TextureMode mode){_textureMode = mode;}
    void onClipLimitChange(bool increase);
    void onSubBlockNumChange(bool increase);
    void onFocusRegionChange(glm::uvec3 step, bool is_scale = true, bool is_enlarge=true);
};
#endif
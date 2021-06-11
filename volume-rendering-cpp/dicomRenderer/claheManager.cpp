#include "claheManager.h"
claheManager::claheManager(GLuint tex_volume, GLuint tex_mask, glm::vec3 volDim){
    m_computer.Init(tex_volume, tex_mask, volDim, outputGrayvals_3D, inputGrayvals_3D, numOrgans);
    _3D_CLAHE = m_computer.Compute3D_CLAHE(numSB_3D, clipLimit3D);
    _FocusedCLAHE = m_computer.ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
    _MaskedCLAHE = m_computer.ComputeMasked3D_CLAHE(clipLimit3D);
    _textureMode = TextureMode::_CLAHE;
    _currTexture = _3D_CLAHE;
}
void claheManager::onClipLimitChange(bool increase){
    clipLimit3D += increase?1:-1;
    if (_textureMode == TextureMode::_CLAHE) {
        _3D_CLAHE = m_computer.Compute3D_CLAHE(numSB_3D, clipLimit3D);
        _currTexture = _3D_CLAHE;
    }
    else if (_textureMode == TextureMode::_FOCUSED) {
        _FocusedCLAHE = m_computer.ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
        _currTexture = _FocusedCLAHE;
    }
    else if (_textureMode == TextureMode::_MASKED) {
        _MaskedCLAHE = m_computer.ComputeMasked3D_CLAHE(clipLimit3D);
        _currTexture = _MaskedCLAHE;
    }
}

void claheManager::onSubBlockNumChange(bool increase){
    if(_textureMode == TextureMode::_CLAHE){
        numSB_3D += glm::uvec3(1);
        if (numSB_3D.x < 1) numSB_3D.x = 1;
        if (numSB_3D.y < 1) numSB_3D.y = 1;
        if (numSB_3D.z < 1) numSB_3D.z = 1;
        _3D_CLAHE = m_computer.Compute3D_CLAHE(numSB_3D, clipLimit3D);
        _currTexture = _3D_CLAHE;
    }
    else if(_textureMode == TextureMode::_FOCUSED){
        if(m_computer.ChangePixelsPerSB(!increase)){
            _FocusedCLAHE = m_computer.ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
            _currTexture = _FocusedCLAHE;
        }
    }
}
void claheManager::onFocusRegionChange(glm::uvec3 step, bool is_scale, bool is_enlarge){
    if(!is_scale){min3D+=step; max3D+=step;}
    else{
        if(is_enlarge){min3D-=step;max3D+=step;}
        else{min3D+=step; max3D-=step;}
    }
    on_focus_change();
}
void claheManager::on_focus_change(){
    GLuint 	newTexture = m_computer.ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
    if (newTexture) {
        _FocusedCLAHE = newTexture;
        _currTexture = newTexture;
    }
}
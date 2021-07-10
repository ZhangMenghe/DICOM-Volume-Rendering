#include "claheManager.h"
claheManager::claheManager(){
    m_computer = new ComputeCLAHE;
}
void claheManager::setCurrentTexMode(dvr::CLAHE_OPTIONS mode){
    _textureMode = mode;
    if (_textureMode == dvr::CLAHE_3D) {
        _currTexture = _3D_CLAHE;
    }
    else if (_textureMode == dvr::CLAHE_FOCUSED) {
        _currTexture = _FocusedCLAHE;
    }
    else if (_textureMode == dvr::CLAHE_MASKED) {
        _currTexture = _MaskedCLAHE;
    }
}
void claheManager::onReset(){
    clipLimit3D = dvr::DEFAULT_CLIP_3D;
    numSB_3D = dvr::DEFAULT_SUBLOCK_NUM;
    min3D = glm::vec3(m_3d_limit)*0.5f - glm::vec3(dvr::DEFAULT_BLOCK_SIZE)*0.5f;
    max3D = min3D + dvr::DEFAULT_BLOCK_SIZE;

    _textureMode = dvr::CLAHE_3D;
    _currTexture = _3D_CLAHE;
    m_dirty = false;ready_to_compute=false;
}
void claheManager::onVolumeDataUpdate(GLuint tex_volume, GLuint tex_mask, glm::vec3 volDim){
    m_3d_limit = glm::uvec3(volDim);
    m_computer->Init(tex_volume, tex_mask, volDim, outputGrayvals_3D, inputGrayvals_3D, numOrgans);

    onReset();

    _3D_CLAHE = m_computer->Compute3D_CLAHE(numSB_3D, clipLimit3D);
    _FocusedCLAHE = m_computer->ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
    _MaskedCLAHE = m_computer->ComputeMasked3D_CLAHE(clipLimit3D);
}
void claheManager::onUpdate(){
    if(m_dirty && ready_to_compute){
        if (_textureMode == dvr::CLAHE_3D) {
            _3D_CLAHE = m_computer->Compute3D_CLAHE(numSB_3D, clipLimit3D);
            _currTexture = _3D_CLAHE;
        }
        else if (_textureMode == dvr::CLAHE_FOCUSED) {
            GLuint 	newTexture = m_computer->ComputeFocused3D_CLAHE(min3D, max3D, clipLimit3D);
            if (newTexture) {
                _FocusedCLAHE = newTexture;
                _currTexture = newTexture;
            }
        }
        else if (_textureMode == dvr::CLAHE_MASKED) {
            _MaskedCLAHE = m_computer->ComputeMasked3D_CLAHE(clipLimit3D);
            _currTexture = _MaskedCLAHE;
        }
        m_dirty = false;ready_to_compute=false;
    }
}
void claheManager::onClipLimitChange(bool increase){
    clipLimit3D += increase?clipStep:-clipStep;
    m_dirty = true;
}

void claheManager::onSubBlockNumChange(bool increase){
    //TODO:TOO SLOW, CRASH, DISABLED FOR WHOLE VOLUME
//    if(_textureMode == dvr::CLAHE_3D){
//        glm::uvec3 old_num_3d = numSB_3D;
//
//        if(increase) numSB_3D += glm::uvec3(1);
//        else numSB_3D -= glm::uvec3(1);
//
//        numSB_3D = glm::clamp(numSB_3D, glm::uvec3(1), glm::uvec3(6));
//        m_dirty = glm::any(glm::notEqual(old_num_3d, numSB_3D));
//    }
//    else
    if(_textureMode == dvr::CLAHE_FOCUSED){
        m_dirty = m_computer->ChangePixelsPerSB(!increase);
    }
}
void claheManager::onFocusRegionChange(glm::uvec3 step, bool is_scale, bool is_enlarge){
    if(!is_scale){
        if(is_enlarge){
            auto old_max3d = max3D;
            max3D = glm::min(max3D+=step, m_3d_limit);
            if(glm::any(glm::notEqual(max3D, old_max3d))){
                min3D = max3D-dvr::DEFAULT_BLOCK_SIZE;
                m_dirty = true;
            }
        }
        else{
            auto old_min3d = min3D;
            min3D = glm::max(min3D-step, glm::uvec3(0));
            if(glm::any(glm::notEqual(min3D, old_min3d))){
                max3D = min3D+dvr::DEFAULT_BLOCK_SIZE;
                m_dirty = true;
            }
        }
    }
    else{
        if(is_enlarge){
            min3D = glm::max(min3D-step, glm::uvec3(0));
            max3D = glm::min(max3D+=step, m_3d_limit);
        }
        else{
            min3D = glm::max(min3D+step, max3D);
            max3D = glm::max(max3D-step, min3D);
        }
        m_dirty = true;
    }
}
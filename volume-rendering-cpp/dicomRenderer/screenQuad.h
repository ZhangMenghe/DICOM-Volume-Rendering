#ifndef DICOM_VOLUME_RENDERING_SCREENQUAD_H
#define DICOM_VOLUME_RENDERING_SCREENQUAD_H

#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class screenQuad {
private:
    static screenQuad* myPtr_;

    Shader qshader_;
    Texture* qtex_ = nullptr;
    GLuint tex_width=0, tex_height=0, m_screen_data_size=0;
    GLuint vao_;
    GLuint frame_buff_ = 0;
    GLubyte* screen_pixels = nullptr;
    bool m_is_recording = false;
public:
    static screenQuad* instance();
    screenQuad();
    void onScreenSizeChange(float width, float height);
    void Draw();
    void Clear();

    //GETTERS
    Texture* getTex(){ return qtex_;}
    glm::vec2 getTexSize(){return glm::vec2(tex_width, tex_height);}
    GLubyte* getCurrentFrame(){return screen_pixels;}

    //SETTERS
    void setOnChangeRecordingStatus(bool is_recording){m_is_recording = is_recording;}
};
#endif //DICOM_VOLUME_RENDERING_SCREENQUAD_H

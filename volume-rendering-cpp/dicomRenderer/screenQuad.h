#ifndef DICOM_VOLUME_RENDERING_SCREENQUAD_H
#define DICOM_VOLUME_RENDERING_SCREENQUAD_H


#include <GLPipeline/Shader.h>
#include <GLPipeline/Texture.h>

class screenQuad {
private:
    static screenQuad* myPtr_;
    const float TEX_HEIGHT = 720.0f;

    Shader qshader_;
    Texture* qtex_ = nullptr;
    float tex_width, tex_height;
    int TEX_ID;
    GLuint vao_;
    GLuint frame_buff_ = 0;
public:
    static screenQuad* instance();
    screenQuad();
    void Draw();
    void Clear();
    Texture* getTex(){ return qtex_;}
    glm::vec2 getTexSize(){return glm::vec2(tex_width, tex_height);}
};


#endif //DICOM_VOLUME_RENDERING_SCREENQUAD_H

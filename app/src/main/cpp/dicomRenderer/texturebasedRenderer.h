#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H

#include <cstring>
#include <GLES3/gl32.h>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>

class texvrRenderer{
private:
    const float scale_inv = 0.5f;
    const size_t NEED_SLCIES = 144;
    int slice_start_idx = 0;

    Shader* shader_;

    std::vector<GLuint> m_VAOs;
    GLuint slice_vao_;

public:
    texvrRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
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
    const float DENSE_FACTOR = 1.0f;
    int dimensions;
    int slice_start_idx = 0;

    Shader* shader_;
    GLuint slice_vao_;

public:
    texvrRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
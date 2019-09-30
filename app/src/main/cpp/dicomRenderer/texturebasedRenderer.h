#ifndef TEXTUREBASED_RENDERER_H
#define TEXTUREBASED_RENDERER_H

#include <cstring>
#include <GLES3/gl32.h>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>

class texvrRenderer{
private:
    const float mVertices[24] = {
            // positions		// texture coords
            -0.5f,  -0.5f, .0,	0.0f, 0.0f, .0,   // top right
            0.5f, -0.5f, .0,	1.0f, 0.0f, .0,  // bottom right
            0.5f, 0.5f, .0,		1.0f, 1.0f, .0,  // bottom left
            -0.5f,  0.5f, .0,	0.0f, 1.0f,  .0  // top left
    };
    const unsigned int indices[6] = {
            0,3,2,
            2,1,0
    };
    const float scale_inv = 0.3f;
    const size_t NEED_SLCIES = 90;
    int slice_start_idx = 0;

    Shader* shader_;

    std::vector<GLuint> m_VAOs;
public:
    texvrRenderer();
    void onCuttingChange(float percent);
    void Draw();
};
#endif
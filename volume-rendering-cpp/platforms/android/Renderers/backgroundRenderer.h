#ifndef RENDERERS_BACKGROUND_RENDERER_H
#define RENDERERS_BACKGROUND_RENDERER_H

#include <platforms/platform.h>
#include <GLPipeline/Shader.h>
static constexpr int kNumVertices = 4;
static constexpr GLfloat kVertices[8] = {
        -1.0f, -1.0f,
        +1.0f, -1.0f,
        -1.0f, +1.0f,
        +1.0f, +1.0f,
};
class backgroundRenderer {
public:
    backgroundRenderer();
    ~backgroundRenderer() = default;

    void Draw(float * uvs_);
    GLuint GetTextureId() const;

private:
    Shader shader_;
    GLuint texture_id_;
    int TEX_ID;
    GLuint vao_, vbo_;
};
#endif

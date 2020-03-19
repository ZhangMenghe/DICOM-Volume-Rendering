#ifndef RENDERERS_BACKGROUND_RENDERER_H
#define RENDERERS_BACKGROUND_RENDERER_H

#include <platforms/platform.h>
#include <GLPipeline/Shader.h>
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

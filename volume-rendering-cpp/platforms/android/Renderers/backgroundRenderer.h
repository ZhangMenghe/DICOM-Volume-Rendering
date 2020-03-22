#ifndef RENDERERS_BACKGROUND_RENDERER_H
#define RENDERERS_BACKGROUND_RENDERER_H

#include <platforms/platform.h>
#include <GLPipeline/Shader.h>

//will be used in arcontroller as well
static constexpr int kNumVertices = 4;
static constexpr GLfloat kVertices[8] = {
        -1.0f, -1.0f,
        +1.0f, -1.0f,
        -1.0f, +1.0f,
        +1.0f, +1.0f,
};
class backgroundRenderer {
public:
    backgroundRenderer(bool screen_baked = false);
    ~backgroundRenderer() = default;

    void Draw(float * uvs_);
    GLuint GetTextureId() const;
    void dirtyPrecompute(){baked_dirty_ = true;}

private:
    Shader shader_;
    GLuint frame_buff_ = 0;
    GLuint texture_id_;
    int TEX_ID;
    GLuint vao_, vbo_;
    bool DRAW_TO_TEXTURE;
    bool baked_dirty_ = true;

    void draw_to_texture();
    void draw_scene();
};
#endif

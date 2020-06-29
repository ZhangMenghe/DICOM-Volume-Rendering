#ifndef VOLUME_RENDERING_CUTPLANERENDERER_H
#define VOLUME_RENDERING_CUTPLANERENDERER_H

#include <platforms/platform.h>
#include <GLPipeline/Shader.h>

class cutplaneRenderer {
public:
    cutplaneRenderer(bool screen_baked = true);
    ~cutplaneRenderer() = default;

    void Draw(glm::mat4 viewproj_mat, glm::vec3 position, glm::vec3 normal_vec);

private:
    Shader* shader_= nullptr;
    GLuint vao_= 0;
    GLuint frame_buff_ = 0;
    bool DRAW_TO_TEXTURE;
    glm::mat4 scale_mat_;

    void draw_to_texture(glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec);
    void draw_scene(glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec);
};


#endif //VOLUME_RENDERING_CUTPLANERENDERER_H

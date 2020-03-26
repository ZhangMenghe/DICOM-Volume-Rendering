#ifndef RENDERERS_PLANE_RENDERER_H
#define RENDERERS_PLANE_RENDERER_H
#include <platforms/platform.h>
#include <GLPipeline/Shader.h>
class PlaneRenderer {
public:
    PlaneRenderer(bool screen_baked = true);
    ~PlaneRenderer() = default;

    void Draw(std::vector<glm::vec3> vertices, std::vector<GLushort> indices,
              glm::mat4 viewproj_mat, glm::mat4 model_mat, glm::vec3 normal_vec,
              glm::vec3 color);

private:
    Shader shader_;
    GLuint frame_buff_ = 0;
    GLuint vao_, vbo_, ibo_;
    bool DRAW_TO_TEXTURE;
    const int MAX_PLANE_VERTICES=100;
    GLuint tex_id;

    void draw_to_texture(std::vector<GLushort> indices,
                         glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec,
                         glm::vec3 color);
    void draw_scene(std::vector<GLushort> indices,
                    glm::mat4 mvp, glm::mat4 model_mat, glm::vec3 normal_vec,
                    glm::vec3 color);
};

#endif
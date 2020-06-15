#ifndef RENDERERS_POINTCLOUD_RENDERER_H
#define RENDERERS_POINTCLOUD_RENDERER_H
#include <platforms/platform.h>
#include <GLPipeline/Shader.h>
class PointCloudRenderer {
public:
    PointCloudRenderer(bool screen_baked = true);
    ~PointCloudRenderer() = default;

    void Draw(glm::mat4 mvp, int point_num, const float * pointcloud);

private:
    Shader shader_;
    GLuint frame_buff_ = 0;
    GLuint vao_, vbo_;
    bool DRAW_TO_TEXTURE;
    int draw_point_num = 0;
    const int MAX_POINT_NUM = 200;

    void draw_to_texture(glm::mat4 mvp);
    void draw_scene(glm::mat4 mvp);
};

#endif
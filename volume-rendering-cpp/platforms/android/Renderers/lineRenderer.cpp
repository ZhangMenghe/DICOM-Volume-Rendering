#include <string>
#include <Manager.h>
#include <dicomRenderer/Constants.h>
#include "lineRenderer.h"
using namespace glm;
lineRenderer::lineRenderer(bool screen_baked)//:PointCloudRenderer(screen_baked){}
{
    DRAW_TO_TEXTURE = screen_baked;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if(!shader_.AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_NAIVE_2D_VERT])
       ||!shader_.AddShader(GL_FRAGMENT_SHADER,  Manager::shader_contents[dvr::SHADER_AR_POINTCLOUD_FRAG])
       ||!shader_.CompileAndLink())
        LOGE("point cloud===Failed to create shader program===");
}

void lineRenderer::Draw(glm::mat4 mvp){
    if(!Manager::ar_intersect) return;

    //calculate far point
    glm::vec3 cpos = Manager::camera->getCameraPosition();
    glm::vec3 dir = Manager::camera->getViewDirection();
    glm::vec3 far_pos = cpos + dir * 10.0f;
    glm::vec2 sp = glm::vec2(mvp * glm::vec4(far_pos, 1.0));
    float points[4] = {
            startPoint.x, startPoint.y,
            sp.x, sp.y
    };
//    LOGE("===TEST %f, %f", startPoint.x, startPoint.y);
    glLineWidth(5.0f);

    draw_point_num = 2;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER,0, 4* sizeof(float),points);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(DRAW_TO_TEXTURE) draw_to_texture(mvp);
    else draw_scene(mvp);
}
void lineRenderer::draw_scene(glm::mat4 mvp){
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uMVP", mvp);
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, draw_point_num);
    glBindVertexArray(0);
    shader_.UnUse();
}
void lineRenderer::setStartPoint(float x, float y) {
    if(initialized) return;
    initialized = true;
    float cx = (x / Manager::screen_w-0.5f) * 2.0f;
    float cy = (y/Manager::screen_h - 0.5f)*2.0f;
    startPoint = glm::vec2(cx,cy );

}


#include <GLPipeline/Primitive.h>
#include <GLPipeline/Mesh.h>
#include "graphRenderer.h"
#include "Constants.h"

GraphRenderer::GraphRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 *MAX_INSTANCES, nullptr, GL_DYNAMIC_DRAW);

    memcpy(indices, single_indices_, sizeof(unsigned int)*12);
    for(int i=1;i<MAX_INSTANCES;i++)
        for(int k=0;k<12;k++)
            indices[12*i+k] = 6*i+single_indices_[k];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*12*MAX_INSTANCES, indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", glm::vec2(1.8f, .15f));
    Shader::Uniform(sp, "uColor", glm::vec4(0.678f, 0.839f, 0.969f, 0.5f));
    shader_.UnUse();
}

void GraphRenderer::getGraphPoints(float values[], float* &points){
    glm::vec2 lb, lm, lt, rb, rm, rt;
    float half_top = values[dvr::TUNE_WIDTHTOP] / 2.0f;
    float half_bottom = std::max(values[dvr::TUNE_WIDTHBOTTOM] / 2.0f, half_top);

    float lb_x = values[dvr::TUNE_CENTER] - half_bottom;
    float rb_x = values[dvr::TUNE_CENTER] + half_bottom;

    lb = glm::vec2(lb_x, .0f);
    rb = glm::vec2(rb_x, .0f);

    lt = glm::vec2(values[dvr::TUNE_CENTER] - half_top, values[dvr::TUNE_OVERALL]);
    rt = glm::vec2(values[dvr::TUNE_CENTER] + half_top, values[dvr::TUNE_OVERALL]);

    float mid_y = values[dvr::TUNE_LOWEST] * values[dvr::TUNE_OVERALL];
    lm = glm::vec2(lb_x, mid_y);
    rm = glm::vec2(rb_x, mid_y);

    if(points) delete points;
    points = new float[12] {
            lb.x, lb.y, lm.x, lm.y, lt.x, lt.y,
            rb.x, rb.y, rm.x, rm.y, rt.x, rt.y
    };
}
//count is the number of points
void GraphRenderer::setUniform(const char* key, const int count, float* data){
    num_of_instances = count/6;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_of_instances*12* sizeof(float), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void GraphRenderer::setData(float* data, int wid){
//    if(wid < num_of_instances){
//        memcpy(vertices + wid*12* sizeof(float), data, 12* sizeof(float));
//        data_dirty = true;
//    }
}
void GraphRenderer::setData(std::vector<float*> data){
    num_of_instances = 1;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_of_instances*12* sizeof(float), data[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    num_of_instances = data.size();
//    if(vertices){delete[] vertices; vertices = nullptr;}
//    vertices = new float[12 * num_of_instances];
//    for(int i=0;i<num_of_instances;i++) memcpy(vertices+i*12* sizeof(float), data[i], 12* sizeof(float));
//    data_dirty = true;
}
void GraphRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", r_scale_);
    Shader::Uniform(sp, "uOffset", r_offset_);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 12*num_of_instances, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader_.UnUse();
    glDisable(GL_BLEND);
}


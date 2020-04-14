#include <GLPipeline/Primitive.h>
#include <GLPipeline/Mesh.h>
#include "graphRenderer.h"
#include "Constants.h"

GraphRenderer::GraphRenderer(std::string vertex_shader, std::string frag_shader)
        :baseQuad(vertex_shader, frag_shader){
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", glm::vec2(1.8f, .15f));
    Shader::Uniform(sp, "uColor", glm::vec4(0.678f, 0.839f, 0.969f, 0.5f));
    shader_.UnUse();
    addInstance();
}
void GraphRenderer::addInstance(){
    GLuint vao_, vbo_, ebo_;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 *MAX_INSTANCES, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*12*MAX_INSTANCES, single_indices_, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vaos_.push_back(vao_); vbos_.push_back(vbo_);
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
void GraphRenderer::removeInstance(int wid){
    if(wid<vaos_.size()){
        vaos_.erase(vaos_.begin()+wid); vbos_.erase(vbos_.begin()+wid);
    }
}
void GraphRenderer::Clear(){
    vaos_.clear(); vbos_.clear();
}

void GraphRenderer::setData(float* data, int wid){
    while(vaos_.size()<=wid) addInstance();
    glBindBuffer(GL_ARRAY_BUFFER, vbos_[wid]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 12* sizeof(float), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void GraphRenderer::setData(std::vector<float*> data){
    while(vaos_.size()<data.size()) addInstance();
    for(int i=0; i<data.size(); i++){
        glBindBuffer(GL_ARRAY_BUFFER, vbos_[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 12* sizeof(float), data[i]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
void GraphRenderer::Draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint sp = shader_.Use();
    Shader::Uniform(sp, "uScale", r_scale_);
    Shader::Uniform(sp, "uOffset", r_offset_);
    for(auto vao:vaos_){
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    shader_.UnUse();
    glDisable(GL_BLEND);
}


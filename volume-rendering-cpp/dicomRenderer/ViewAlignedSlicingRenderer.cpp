#include <vrController.h>
#include "ViewAlignedSlicingRenderer.h"
#include <GLPipeline/Primitive.h>
#include "screenQuad.h"
#include <algorithm>

using namespace glm;
ViewAlignedSlicingRenderer::ViewAlignedSlicingRenderer()
        :baseDicomRenderer(){
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_VIEW_ALIGNED_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_VIEW_ALIGNED_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("ViewAligned ===Failed to create shader program===");

    for(int i=0;i<MAX_DIMENSIONS;i++){
        glGenVertexArrays(1, &m_vaos[i]);
        unsigned int EBO;
        glGenBuffers(1, &m_vbos[i]);
        glGenBuffers(1, &EBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(m_vaos[i]);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*12, m_indices_data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

float RayPlane(vec3 ro, vec3 rd, vec3 pp, vec3 pn) {
    float d = dot(pn, rd);
    float t = dot(pp - ro, pn);
    return d > 1e-5 ? (t / d) : (t > .0 ? 1e5 : -1e5);
}
void PlaneBox(vec3 aabb_min, vec3 aabb_max, vec3 pp, vec3 pn, std::vector<vec3>& out_points){
    float t;
    glm::vec3 rd;
    //axis-x
    rd = vec3(aabb_max.x - aabb_min.x, .0f, .0f);
    vec3 ro_x[4] = {
            aabb_min,
            vec3(aabb_min.x, aabb_max.y, aabb_min.z),
            vec3(aabb_min.x, aabb_min.y, aabb_max.z),
            vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    };
    for(auto ro:ro_x){
        t = RayPlane(ro, rd, pp, pn);
        if(t >= .0 && t <= 1.0f) out_points.push_back(ro+rd*t);
    }

    //axis-y
    rd = vec3(.0f, aabb_max.y-aabb_min.y,.0f);
    vec3 ro_y[4] = {
            vec3(aabb_min.x, aabb_min.y, aabb_min.z),
            vec3(aabb_max.x, aabb_min.y, aabb_min.z),
            vec3(aabb_min.x, aabb_min.y, aabb_max.z),
            vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    };
    for(auto ro:ro_y){
        t = RayPlane(ro, rd, pp, pn);
        if(t >= .0 && t <= 1.0f) out_points.push_back(ro+rd*t);
    }

    //axis-z
    rd = vec3(.0f, .0f,aabb_max.z-aabb_min.z);
    vec3 ro_z[4] = {
            vec3(aabb_min.x, aabb_min.y, aabb_min.z),
            vec3(aabb_max.x, aabb_min.y, aabb_min.z),
            vec3(aabb_min.x, aabb_max.y, aabb_min.z),
            vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    };
    for(auto ro:ro_z){
        t = RayPlane(ro, rd, pp, pn);
        if(t >= .0 && t <= 1.0f) out_points.push_back(ro+rd*t);
    }
}

void SortPoints(std::vector<vec3>& points, vec3 pn){
    if (points.empty()) return;

    auto origin = points[0];

    std::sort(points.begin(), points.end(), [origin, pn](const glm::vec3 &lhs, const glm::vec3 &rhs) -> bool
    {
        glm::vec3 v = glm::cross(lhs - origin, rhs - origin);
        return glm::dot(v, pn) > .0f;
    } );
}
void AssembleVerticesAndCenterlize(const std::vector<vec3>& points, float*& vertices, int& vertex_num){
    vertex_num = points.size();
    if(vertex_num < 3) return;

//    vertices = new float[vertex_num * 3];
//    glm::vec3 center = glm::vec3(.0f);
    for(int i=0; i<vertex_num; i++){
//        center+=points[i];
        vertices[6*i]=points[i].x;vertices[6*i+1]=points[i].y;vertices[6*i+2]=points[i].z;
        //todo: texcoord
    }
//    center/=vertex_num;
//    for(int i=0; i<vertex_num; i++){
//        vertices[6*i]-=center.x; vertices[6*i+1]-=center.y;vertices[6*i+2]-=center.z;
//    }
}
void ViewAlignedSlicingRenderer::update_instance_data(glm::mat4 model_mat){
    //Step 1: Transform the volume bounding box vertices into view coordinates using the modelview matrix
    // Volume view vertices
    float volume_view_vertices[8][3];

    glm::mat4 model_view = Manager::camera->getViewMat() * model_mat;
    glm::mat4 model_view_inv = glm::inverse(model_view);

    for(int i=0;i<8;i++){
        glm::vec4 v(cuboid[3*i], cuboid[3*i+1], cuboid[3*i+2], 1.0f);
        glm::vec4 vv = model_view * v;
        volume_view_vertices[i][0] = vv.x / vv.w; volume_view_vertices[i][1] = vv.y / vv.w;volume_view_vertices[i][2] = vv.z / vv.w;
    }
    //Step 2: Find the minimum and maximum z coordinates of the transformed vertices. Compute the number of sampling planes used between these two values using equidistant spacing from the view origin. The sampling distance is computed from the voxel size and current sampling rate.
    float z_min = std::numeric_limits<float>::max(), z_max = -z_min;
    int z_min_id = -1, z_max_id = -1;
    for(int i=0;i<8;i++){
        if(volume_view_vertices[i][2] > z_max){
            z_max = volume_view_vertices[i][2]; z_max_id=i;
        }else if(volume_view_vertices[i][2] < z_min){
            z_min= volume_view_vertices[i][2];z_min_id=i;
        }
    }

    glm::vec4 zmin_view = glm::vec4(.0f,.0f, z_min,1.0), zmax_view = glm::vec4(.0,.0,z_max,1.0);
    glm::vec4 zmin_modelv4 = model_view_inv* zmin_view, zmax_modelv4 = model_view_inv* zmax_view;
    glm::vec3 zmin_model = vec3(zmin_modelv4.x, zmin_modelv4.y, zmin_modelv4.z) / zmin_modelv4.w;
    glm::vec3 zmax_model = vec3(zmax_modelv4.x, zmax_modelv4.y, zmax_modelv4.z) / zmax_modelv4.w;
    glm::vec3 distance = zmax_model - zmin_model;
    float slice_distance = glm::length(distance);
    float slice_spacing = 0.1f;

    m_slice_num = int(slice_distance/ slice_spacing);

    //For each plane in front-to-back or back-to-front order
    vec3 aabb_min(-0.5f), aabb_max(0.5f);
    glm::vec4 pnv4 = model_view_inv* glm::vec4(.0, .0, -1.0f, 1.0f);
    glm::vec3 pn = glm::normalize(vec3(pnv4.x, pnv4.y, pnv4.z) / pnv4.w);
//    glm::vec4 ppv4 = model_view_inv*glm::vec4(volume_view_vertices[z_min_id][0], volume_view_vertices[z_min_id][1], volume_view_vertices[z_min_id][2], 1.0f);
//    glm::vec3 pp = vec3(ppv4.x, ppv4.y, ppv4.z) / ppv4.w;
    glm::vec3 pp = vec3(cuboid[3*z_min_id], cuboid[3*z_min_id+1], cuboid[3*z_min_id+2]);
    float* vertices = new float[6 * 6];
    int vertex_num;

    for(int slice = 0; slice<m_slice_num; slice++){
        pp+=pn*slice_spacing;
        // a. Test for intersections with the edges of the bounding box. Add each intersection point to a temporary vertex list. Up to six intersections are generated, so the maximum size of the list is fixed.
        std::vector<vec3> polygon_points;
        PlaneBox(aabb_min, aabb_max, pp, pn, polygon_points);

        //b. compute center and sort them in counter-clockwise
        //c. Tessellate the proxy polygon
        SortPoints(polygon_points, pn);
        AssembleVerticesAndCenterlize(polygon_points, vertices, vertex_num);
        m_indice_num[slice] = (vertex_num - 2) * 3;

        //update to gpu
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[slice]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6* vertex_num, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    delete []vertices;
}
void ViewAlignedSlicingRenderer::draw_scene(glm::mat4 model_mat){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
    Shader::Uniform(sp, "uSampler_baked", dvr::BAKED_TEX_ID);

    Shader::Uniform(sp, "uMVP", Manager::camera->getVPMat() * model_mat);
//    Shader::Uniform(sp, "u_cut", Manager::param_bool[dvr::CHECK_CUTTING]);

    for(int i=0; i<m_slice_num; i++) {
        glBindVertexArray(m_vaos[i]);
        glDrawElements(GL_TRIANGLES, m_indice_num[i], GL_UNSIGNED_INT, 0);
    }
    shader_->UnUse();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void ViewAlignedSlicingRenderer::Draw(bool pre_draw, glm::mat4 model_mat){
    update_instance_data(model_mat);
//    float vertices[24] = {
//            -0.5f,-0.5f,0.f,   0.0f,0.0f,1.0f,
//            0.5f,-0.5f,0.f,    1.0f,0.0f,1.0f,
//            0.5f,0.5f,0.f,	    1.0f,1.0f,1.0f,
//            -0.5f,0.5f,0.f,    0.0f,1.0f,1.0f,
//    };
//    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6* 4, vertices);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(pre_draw || Manager::param_bool[dvr::CHECK_AR_ENABLED]) draw_baked(model_mat);
    else draw_scene(model_mat);
}

void ViewAlignedSlicingRenderer::draw_baked(glm::mat4 model_mat) {
    if(!Manager::param_bool[dvr::CHECK_AR_ENABLED] && !baked_dirty_) return;
    if(!frame_buff_) Texture::initFBO(frame_buff_, screenQuad::instance()->getTex(), nullptr);
    //render to texture
    glm::vec2 tsize = screenQuad::instance()->getTexSize();
    glViewport(0, 0, tsize.x, tsize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buff_);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw_scene(model_mat);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    baked_dirty_ = false;
}

void ViewAlignedSlicingRenderer::setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale){
//    baseDicomRenderer::setDimension(vol_dim, vol_scale);
//
//    dimensions = int(vol_dim.z * DENSE_FACTOR);dimension_inv = 1.0f / dimensions;
//    vol_thickness_factor = vol_scale.z;
//    update_instance_data(vbo_front, true);
//    update_instance_data(vbo_back, false);
}
void ViewAlignedSlicingRenderer::setCuttingPlane(float percent){
//    cut_id = int(dimensions * percent);
//    baked_dirty_ = true;
}
void ViewAlignedSlicingRenderer::setCuttingPlaneDelta(int delta){
//    cut_id = ((int)fmax(0, cut_id + delta))%dimensions;
//    baked_dirty_ = true;
}
#include <vrController.h>
#include "ViewAlignedSlicingRenderer.h"
#include <GLPipeline/Primitive.h>
#include "screenQuad.h"
#include <algorithm>

using namespace glm;
namespace {
    const unsigned int m_single_indices_data[12] = {
            0,1,2,0,2,3,0,3,4,0,4,5
    };
}
ViewAlignedSlicingRenderer::ViewAlignedSlicingRenderer()
        :baseDicomRenderer(){
    //program
    shader_ = new Shader();
    if(!shader_->AddShader(GL_VERTEX_SHADER, Manager::shader_contents[dvr::SHADER_VIEW_ALIGNED_VERT])
       ||!shader_->AddShader(GL_FRAGMENT_SHADER, Manager::shader_contents[dvr::SHADER_VIEW_ALIGNED_FRAG])
       ||!shader_->CompileAndLink())
        LOGE("ViewAligned ===Failed to create shader program===");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_vao);

    m_vertices = new float[18 * MAX_DIMENSIONS];
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * MAX_DIMENSIONS, nullptr, GL_DYNAMIC_DRAW);

    m_indices = new unsigned int[12*MAX_DIMENSIONS];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*12*MAX_DIMENSIONS, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float tmp[] ={Manager::indiv_rendering_params[dvr::VIEW_ALIGN_SLICING]};
    setRenderingParameters(tmp);
}

float RayPlane(vec3 ro, vec3 rd, vec3 pp, vec3 pn) {
    float d = dot(pn, rd);
    float t = dot(pp - ro, pn);
    return fabs(d) > 1e-5 ? (t / d) : (t > .0 ? 1e5 : -1e5);
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

void ViewAlignedSlicingRenderer::UpdateVertices(glm::mat4 model_mat){
    baseDicomRenderer::UpdateVertices(model_mat);
    glm::mat4 model_view = Manager::camera->getViewMat() * model_mat;
    glm::mat4 model_view_inv = glm::inverse(model_view);

    glm::vec4 pnv4 = model_view_inv* glm::vec4(.0, .0, 1.0f, 1.0f);
    glm::vec3 pn = glm::normalize(vec3(pnv4.x, pnv4.y, pnv4.z) / pnv4.w);
//    LOGE("====PN : %f, %f, %f", pn.x, pn.y, pn.z);

//    if(dvr::VIEW_ALIGNED_LAZY_UPDATE){
//        if(abs(pn.x) > abs(pn.y) && abs(pn.x) > abs(pn.z)){pn.x = pn.x > .0f?1.0:-1.0;pn.y=0;pn.z=0;}
//        else if(abs(pn.y) > abs(pn.x) && abs(pn.y) > abs(pn.z)){pn.y = pn.y > .0f?1.0:-1.0;pn.x=0;pn.z=0;}
//        else if(abs(pn.z) > abs(pn.y) && abs(pn.z) > abs(pn.x)){pn.z = pn.z > .0f?1.0:-1.0;pn.y=0;pn.x=0;}
//        if(glm::all(glm::equal(m_last_vec3, pn))) return;
//        m_last_vec3 = pn;
        //    LOGE("====PN : %f, %f, %f", pn.x, pn.y, pn.z);
        //    LOGE("====pp : %f, %f, %f", pp.x, pp.y, pp.z);
//    }

    //Step 1: Transform the volume bounding box vertices into view coordinates using the modelview matrix
    // Volume view vertices
    float volume_view_vertices[8][3];

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

    m_slice_num = min(int(float(dimensions_origin) * slice_distance * m_sampling_rate), MAX_DIMENSIONS);
//    m_slice_amount+=m_slice_num;
//    m_slice_count++;
//    if(m_slice_count > 300){
//        LOGE("=======SLICE NUM %f", m_slice_amount/m_slice_count);
//        m_slice_amount = 0; m_slice_count=0;
//    }
    float slice_spacing = 1.0f/float(m_slice_num);

    //For each plane in front-to-back or back-to-front order
    vec3 aabb_min(-0.5f), aabb_max(0.5f);
    int id = 3*z_min_id;
    glm::vec3 pp = vec3(cuboid[id], cuboid[id+1], cuboid[id+2]);

//    float vertices[18] = {.0f};
    int s_vertex_num, s_indice_num;
    m_indices_num = 0; m_vertices_num=0;
    for(int slice = 0, offset=0, off_ind=0; slice<m_slice_num; slice++,pp+=pn*slice_spacing){
        // a. Test for intersections with the edges of the bounding box. Add each intersection point to a temporary vertex list. Up to six intersections are generated, so the maximum size of the list is fixed.
        std::vector<vec3> polygon_points;
        PlaneBox(aabb_min, aabb_max, pp, pn, polygon_points);

        s_vertex_num = polygon_points.size();
        if(s_vertex_num < 3) continue;

        s_indice_num = (s_vertex_num - 2) * 3;
        for(int i=0;i<s_indice_num;i++)m_indices[off_ind+i]=m_single_indices_data[i]+m_vertices_num;
        m_vertices_num+=s_vertex_num;
        m_indices_num+=s_indice_num;

        //b. compute center and sort them in counter-clockwise
        //c. Tessellate the proxy polygon
        SortPoints(polygon_points, pn);

        for(int i=0; i<s_vertex_num; i++){
            m_vertices[offset+3*i]=polygon_points[i].x;m_vertices[offset+3*i+1]=polygon_points[i].y;m_vertices[offset+3*i+2]=polygon_points[i].z;}
        offset+=s_vertex_num*3;
        off_ind+=s_indice_num;
    }
    //update to gpu
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3* m_vertices_num, m_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_indices_num, m_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void ViewAlignedSlicingRenderer::draw_scene(glm::mat4 model_mat){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    GLuint sp = shader_->Use();

    glActiveTexture(GL_TEXTURE0 + dvr::BAKED_TEX_ID);
    glBindTexture(GL_TEXTURE_3D, vrController::instance()->getBakedTex());
    Shader::Uniform(sp, "uSampler_baked", dvr::BAKED_TEX_ID);

    Shader::Uniform(sp, "uMVP", Manager::camera->getVPMat() * model_mat);

    if(Manager::IsCuttingEnabled())shader_->EnableKeyword("CUTTING_PLANE");
    else shader_->DisableKeyword("CUTTING_PLANE");

    vrController::instance()->setCuttingParams(sp);
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices_num, GL_UNSIGNED_INT, 0);

    shader_->UnUse();
    glFrontFace(GL_CCW);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}

void ViewAlignedSlicingRenderer::Draw(bool pre_draw, glm::mat4 model_mat){
//    update_instance_data(model_mat);
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
void ViewAlignedSlicingRenderer::setRenderingParameters(float* values){
    if(values[0]!=m_sampling_rate){
        vertices_dirty_ = true;
        m_sampling_rate = values[0];
    }
}
//void ViewAlignedSlicingRenderer::setDimension(glm::vec3 vol_dim, glm::vec3 vol_scale){
//    dimensions = vol_dim.z;
//}
//void ViewAlignedSlicingRenderer::setCuttingPlane(float percent){
////    cut_id = int(dimensions * percent);
////    baked_dirty_ = true;
//}
//void ViewAlignedSlicingRenderer::setCuttingPlaneDelta(int delta){
////    cut_id = ((int)fmax(0, cut_id + delta))%dimensions;
////    baked_dirty_ = true;
//}
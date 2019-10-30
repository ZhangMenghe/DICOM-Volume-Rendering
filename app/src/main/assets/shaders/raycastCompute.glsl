#version 310 es

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image2D destTex;

uniform vec2 u_con_size;
uniform float u_fov;
uniform mat4 u_MV_inv;
uniform vec3 uCamposObjSpace;

const float constantNCP = 1.0;
bool RayCube(vec3 ro, vec3 rd, vec3 extents) {
    vec3 boxmin = -extents;
    vec3 boxmax = extents;

    // compute intersection of ray with all six bbox planes
    vec3 invR = 1.0f / rd;
    vec3 tbot = invR * (boxmin - ro);
    vec3 ttop = invR * (boxmax - ro);

    // re-order intersections to find smallest and largest on each axis

    vec3 tmin = vec3(100000.0f);
    vec3 tmax = vec3(0.0f);

    tmin = min(tmin, min(ttop, tbot));
    tmax = max(tmax, max(ttop, tbot));

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));

    return smallest_tmax > largest_tmin;
}
vec4 tracing(float u, float v){
    vec4 color = vec4(.0);
    // s1: calculate eye ray
    float tangent = tan(u_fov / 2.0); // angle in radians
    float ar = (float(u_con_size.x) / u_con_size.y);
//    vec4 ro = u_MV_inv * vec4(.0,.0,.0,1.0);
    vec3 ro = uCamposObjSpace;
    vec3 rd = vec3(normalize(u_MV_inv * normalize(vec4(u * tangent * ar, v * tangent, -constantNCP, 0.0f))));
    if(!RayCube(ro, rd, vec3(0.5))) return color;

    return vec4(0.8,0.8,0.0,1.0);
}
void main() {
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    float cx = float(storePos.x); float cy = float(storePos.y);
    if ( cx >= u_con_size.x ||  cy >= u_con_size.y) return;
    cy = u_con_size.y - cy;

    vec2 uv = (vec2(cx, cy) + 0.5) / u_con_size * 2.0 - 1.0;

    imageStore(destTex, ivec2(cx, cy), tracing(uv.x, uv.y));
}

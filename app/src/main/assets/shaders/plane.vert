#version 310 es
#extension GL_EXT_shader_io_blocks:require

precision mediump float;
layout (location = 0) in vec3 aPosition;

out VS_OUT {
    float v_alpha;
    vec2 v_texcoord;
} vs_out;

uniform mat4 uMVP;
uniform mat4 uModelMat;
uniform vec3 uNormal;

const vec3 arbitrary = vec3(1.0, 1.0, 0.0);

void main(){
    vec4 local_pos = vec4(aPosition.x, 0.0, aPosition.y, 1.0);
    vec4 world_pos = uModelMat * local_pos;

    // Construct two vectors that are orthogonal to the normal.
    // This arbitrary choice is not co-linear with either horizontal
    // or vertical plane normals.
    vec3 vec_u = normalize(cross(uNormal, arbitrary));
    vec3 vec_v = normalize(cross(uNormal, vec_u));

    // Project vertices in world frame onto vec_u and vec_v.
    vs_out.v_texcoord = vec2(dot(world_pos.xyz, vec_u), dot(world_pos.xyz, vec_v));
    vs_out.v_alpha = aPosition.z;

    gl_Position = uMVP *local_pos;
}
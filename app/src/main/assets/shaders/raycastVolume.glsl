#version 310 es
#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image3D destTex;

vec4 Sample(ivec3 pos){
    return vec4(1.0, .0, .0, 1.0);
}
void main(){
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    vec4 final_color = Sample(storePos);
    imageStore(destTex, storePos, final_color);
}

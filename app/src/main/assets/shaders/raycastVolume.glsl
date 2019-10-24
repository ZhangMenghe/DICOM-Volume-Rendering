#version 310 es

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image3D destTex;

vec4 Sample(ivec3 pos){
    vec2 sc = imageLoad(srcTex, pos).rg;
    vec4 color = vec4(.0);
//    if(ub_invert)
//        sc.r = 1.0 - sc.r;
    color.rgb = vec3(sc.r);
    color.a = 1.0;

//    if(ub_maskon)
//        if(sc.g > 0.01) color.gb = vec2(.0);
//    if(ub_maskonly)
//        color.a *= sc.g;
    return color;
}
void main(){
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    vec4 final_color = Sample(storePos);
    imageStore(destTex, storePos, final_color);
}

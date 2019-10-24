#version 310 es
#pragma multi_compile MASKON ORGANS_ONLY
//#pragma multi_compile LIGHT_DIRECTIONAL LIGHT_SPOT LIGHT_POINT

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image3D destTex;

vec4 Sample(ivec3 pos){
    vec2 sc = imageLoad(srcTex, pos).rg;
    vec4 color = vec4(1.0);
    color.rgb = vec3(sc.r);
#ifdef MASKON
    if(sc.g > 0.01) color.gb = vec2(.0);
#endif

#ifdef ORGANS_ONLY
    color.a = 0.0;//*= sc.g;
#endif
    return color;
}

float Light(vec3 p){
    return 1.0;
}
void main(){
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    vec4 final_color = Sample(storePos);
    final_color.rgb *= Light(vec3(storePos));
    imageStore(destTex, storePos, final_color);
}

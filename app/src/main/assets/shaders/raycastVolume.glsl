#version 310 es

#pragma multi_compile UPDATE_RAY_BAKED
#pragma multi_compile ORGANS_ONLY
#pragma multi_compile TRANSFER_COLOR MASKON

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image3D destTex_tex;
layout(binding = 2, rgba8)writeonly uniform mediump image3D destTex_ray;

//Uniforms for texture_baked
struct OpacityAdj{
    float overall;//0-1
    float lowbound; //slope adj, 0-1
    float cutoff;//0,1
};
uniform OpacityAdj uOpacitys;

//Uniforms for ray_baked
uniform float u_val_threshold;
uniform float u_brightness;

float CURRENT_INTENSITY;

const float START_H_VALUE = 0.1667;
const float BASE_S_VALUE = 0.7;
const float BASE_S_H = 0.6667;//pure blue
const float BASE_V_VALUE = 0.8;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme(float gray){
    // transfer a gray-scale from 0-1 to proper rgb value
    float h = (1.0 - START_H_VALUE) * gray + START_H_VALUE;
    float off_h = h - BASE_S_H;
    float s = off_h>.0? BASE_S_VALUE + off_h / (1.0 - BASE_S_H) * 0.3: BASE_S_VALUE + off_h / (BASE_S_H - START_H_VALUE) * 0.5;
    float v = off_h >.0? BASE_V_VALUE: BASE_V_VALUE + off_h / (BASE_S_H - START_H_VALUE)*0.3;
    return hsv2rgb(vec3(h,s,v));
}
vec4 UpdateTextureBased(vec4 sampled_color){
    float alpha = CURRENT_INTENSITY * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
    alpha = (CURRENT_INTENSITY < uOpacitys.cutoff)?.0:alpha*sampled_color.a;
    return vec4(sampled_color.rgb, alpha*uOpacitys.overall);
}
vec4 UpdateRaybased(vec4 sampled_color){
    float alpha = CURRENT_INTENSITY + u_val_threshold - 0.5;
    if(sampled_color.r==CURRENT_INTENSITY) {
        float up_inten = clamp(alpha * u_brightness / 250.0, 0.0, 1.0);
        return vec4(vec3(up_inten), up_inten * sampled_color.a);
    }
    return vec4(sampled_color.rgb, alpha*sampled_color.a);
}
vec4 Sample(ivec3 pos){
    vec2 sc = imageLoad(srcTex, pos).rg;
    CURRENT_INTENSITY = sc.r;
    vec4 color = vec4(vec3(CURRENT_INTENSITY), 1.0);

#ifdef TRANSFER_COLOR
    color.rgb = transfer_scheme(sc.r);
#elif defined MASKON
    if(sc.g > 0.01) color.gb = vec2(.0);
#endif

#ifdef ORGANS_ONLY
    color.a *= sc.g;
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
    #ifdef UPDATE_RAY_BAKED
        vec4 ray_color = UpdateRaybased(final_color);
        imageStore(destTex_ray, storePos, ray_color);
    #else
        vec4 tex_color = UpdateTextureBased(final_color);
        imageStore(destTex_tex, storePos, tex_color);
    #endif
}

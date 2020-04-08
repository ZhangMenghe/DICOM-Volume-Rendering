#version 310 es

#pragma multi_compile SHOW_ORGANS
#pragma multi_compile COLOR_GRAYSCALE COLOR_HSV COLOR_BRIGHT
#pragma multi_compile LIGHT_DIRECTIONAL LIGHT_SPOT LIGHT_POINT

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, r32ui)readonly uniform mediump uimage3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image3D destTex;

//Uniforms for texture_baked
struct OpacityAdj{
    float overall;//0-1
    float lowbound; //slope adj, 0-1
    float cutoff;//0,1
};
//shaderd by tex and ray, but mutually exclusive
//uniform OpacityAdj uOpacitys;
uniform vec2 u_opacity[6];

float CURRENT_INTENSITY;
uint MASKS_;

//last bit indicates body(which doesn't belong to organs)
uniform uint u_maskbits;// = uint(31);
uniform uint u_organ_num;// = uint(4);

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
uvec3 transfer_scheme(float gray){
    return uvec3(hsv2rgb(vec3(gray, 1.0, 1.0)) * 255.0);
}
uvec3 transfer_scheme(float cat, float gray){
    return uvec3(hsv2rgb(vec3(cat, 1.0, gray)) * 255.0);
}
//hot to color. H(0~180)
uvec3 bright_scheme(float gray){
    return uvec3(hsv2rgb(vec3(gray * 180.0 / 255.0, 1.0, 1.0)) * 255.0);
}
uint UpdateOpacityAlpha(uint sampled_alpha){
    float falpha = float(sampled_alpha) * 0.003921;
    float alpha = CURRENT_INTENSITY;
    vec2 lb = u_opacity[0], rb = u_opacity[3];
    if(alpha < lb.x || alpha > rb.x) return uint(0);
    vec2 lm = u_opacity[1], lt =u_opacity[2];
    vec2 rm = u_opacity[4], rt =u_opacity[5];
    float k = (lt.y - lm.y)/(lt.x - lm.x);
    if(alpha < lt.x) alpha*= k*(alpha - lm.x)+lm.y;
    else if(alpha < rt.x) alpha*=rt.y;
    else alpha*= -k *(alpha - rm.x)+rm.y;
    return uint(falpha * alpha * 255.0);
}

uvec4 show_organs(uvec4 color){
    uint alpha = uint(0);
    //organs
    for(uint i=uint(0); i<u_organ_num; i++){
        if( ((u_maskbits>> uint(i + uint(1))) & uint(1)) == uint(0)) continue;
        uint cbit = (MASKS_>> uint(i)) & uint(1);
        alpha = alpha | cbit;
        if(cbit == uint(1)){
            color.rgb = transfer_scheme(float(i) / float(u_organ_num), CURRENT_INTENSITY);
            break;
        }
    }
    // body
    if(alpha == uint(0)){
        if( ( u_maskbits & uint(1) ) == uint(1)) 
        {alpha = uint(1); color.rgb = uvec3(color.r);}
    }
    color.a*= alpha;
    return color;
}
uvec4 Sample(ivec3 pos){
    uint value = imageLoad(srcTex, pos).r;
    //lower part as color
    uvec4 color = uvec4(uvec3(value&uint(0xffff)), 255);
    CURRENT_INTENSITY = float(color.r) * 0.003921;
    MASKS_ = value>>uint(16);
    return color;
}
uvec4 post_process(uvec4 color){
    #ifdef COLOR_HSV
        color.rgb = transfer_scheme(CURRENT_INTENSITY);
    #elif defined(COLOR_BRIGHT)
        color.rgb = bright_scheme(CURRENT_INTENSITY);
    #endif

    #ifdef SHOW_ORGANS
        //upper part as mask
        color = show_organs(color);
    #endif
    return color;
}

void main(){
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    uvec4 sample_color = Sample(storePos);
    uint alpha = UpdateOpacityAlpha(sample_color.a);
    uvec4 ufc = post_process(uvec4(sample_color.rgb, alpha));
    imageStore(destTex, storePos, vec4(ufc) * 0.003921);
}


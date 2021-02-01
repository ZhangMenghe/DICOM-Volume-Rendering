#version 310 es

#pragma multi_compile COLOR_GRAYSCALE COLOR_HSV COLOR_BRIGHT COLOR_FIRE COLOR_CET_L08

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0, rgba8)writeonly uniform mediump image2D destTex;

//opacity
uniform vec2 u_opacity[60];
uniform int u_widget_num;
uniform int u_visible_bits;

//contrast
uniform float u_contrast_low;
uniform float u_contrast_high;
uniform float u_brightness;

//color scheme
uniform int u_hex_color_scheme[256];
uniform vec2 u_tex_size;


// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 hex2rgb(int hex){
    return vec3(
    (float((hex >> 16) & 0xFF)) / 255.0,
    (float((hex >> 8) & 0xFF)) / 255.0,
    (float((hex) & 0xFF)) / 255.0
    );
}
vec3 transfer_scheme(float gray){
    return hsv2rgb(vec3(gray, 1.0, 1.0));
}

//hot to color. H(0~180)
vec3 bright_scheme(float gray){
    return hsv2rgb(vec3((1.0 -gray) * 180.0 / 255.0, 1.0, 1.0));
}
vec3 AdjustContrastBrightness(vec3 color){
    float cr = 1.0 / (u_contrast_high - u_contrast_low);
    color = clamp(cr * (color - vec3(u_contrast_low)), .0, 1.0);
    return clamp(color + u_brightness*2.0 - 1.0, .0, 1.0);
}
vec3 TransferColor(float intensity){
    vec3 color;
    #ifdef COLOR_GRAYSCALE
    color = vec3(intensity);
//    #elif defined(COLOR_HSV)
//    color = transfer_scheme(intensity);
//    #elif defined(COLOR_BRIGHT)
//    color = bright_scheme(intensity);
    #else
    color = hex2rgb(u_hex_color_scheme[int(intensity * 256.0)]);
    #endif
//    return color;
    return AdjustContrastBrightness(color);
}
float get_intensity(int uid_offset, float posx){
    vec2 lb = u_opacity[uid_offset], rb = u_opacity[uid_offset+3];
    if(posx < lb.x || posx > rb.x) return .0;

    vec2 lm = u_opacity[uid_offset+1], lt =u_opacity[uid_offset+2];
    vec2 rm = u_opacity[uid_offset+4], rt =u_opacity[uid_offset+5];
    float k = (lt.y - lm.y)/(lt.x - lm.x);
    if(posx < lt.x) return k*(posx - lm.x)+lm.y;
    if(posx < rt.x) return rt.y;
    return -k *(posx - rm.x)+rm.y;
}
vec4 get_mixture(float posx, vec3 gray){
    return vec4(TransferColor(posx), gray.r);
}

void main(){
    float xpos = float(gl_GlobalInvocationID.x) / u_tex_size.x;
    uint unit_size = uint(u_tex_size.y / 3.0);

    //intensity
    float gray = .0;
    for(int i=0; i<u_widget_num; i++){
        if(((u_visible_bits >> i) & 1) == 1) gray = max(gray, get_intensity(6*i, xpos));
    }
    for(uint y=uint(0); y<unit_size; y++){
        ivec2 storePos = ivec2(gl_GlobalInvocationID.x, y);
        imageStore(destTex, storePos, vec4(gray));
    }
    //color scheme
    vec4 color = vec4(TransferColor(xpos), 1.0);
    for(uint y=uint(2)*unit_size; y<uint(u_tex_size.y); y++){
        ivec2 storePos = ivec2(gl_GlobalInvocationID.x, y);
        imageStore(destTex, storePos, color);
    }
    //mixture
    for(uint y=unit_size; y<uint(2)*unit_size; y++){
        ivec2 storePos = ivec2(gl_GlobalInvocationID.x, y);
        imageStore(destTex, storePos, vec4(color.rgb, gray));
    }
}
#version 300 es

precision mediump float;
uniform int uScheme;
uniform vec2 u_opacity[6];

out vec4 gl_FragColor;
in vec2 vTexcoord;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme_hsv(float gray){
    if(uScheme == 1) return vec3(gray, 1.0, 1.0);
    return vec3(gray * 180.0 / 255.0, 1.0, 1.0);
}
vec3 get_intensity(float posx){
    vec2 lb = u_opacity[0], rb = u_opacity[3];
    if(posx < lb.x || posx > rb.x) return vec3(.0);

    vec2 lm = u_opacity[1], lt =u_opacity[2];
    vec2 rm = u_opacity[4], rt =u_opacity[5];
    float k = (lt.y - lm.y)/(lt.x - lm.x);
    if(posx < lt.x) return vec3(k*(posx - lm.x)+lm.y);
    if(posx < rt.x) return vec3(rt.y);
    return vec3(-k *(posx - rm.x)+rm.y);
}
vec4 get_mixture(float posx, vec3 gray){
    if(uScheme == 0) return vec4(gray.r);
    vec3 color_hsv = transfer_scheme_hsv(posx);
    return vec4(hsv2rgb(color_hsv),gray.r);
}
void main(){
    if(vTexcoord.y >0.66){
        if(uScheme == 0) gl_FragColor = vec4(vec3(vTexcoord.x), 1.0);
        else gl_FragColor = vec4(hsv2rgb(transfer_scheme_hsv(vTexcoord.x)), 1.0);
    }else{
        vec3 gray = get_intensity(vTexcoord.x);
        if(vTexcoord.y<0.33) gl_FragColor = vec4(gray, 1.0);
        else gl_FragColor = get_mixture(vTexcoord.x, gray);
    }
}
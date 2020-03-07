#version 300 es
precision mediump float;
out vec4 gl_FragColor;
in vec2 vTexcoord;

float START_H_VALUE = 0.1667;
float BASE_S_VALUE = 0.7;
float BASE_S_H = 0.6667;//pure blue
float BASE_V_VALUE = 0.8;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme(float gray){
    // transfer a gray-scale from 0-1 to proper rgb value
    // float range = 0.833;
    // float h = (1.0 - START_H_VALUE) * gray + START_H_VALUE;
    // float off_h = h - BASE_S_H;
    // float s = off_h>.0? BASE_S_VALUE + off_h / (1.0 - BASE_S_H) * 0.3: BASE_S_VALUE + off_h / (BASE_S_H - START_H_VALUE) * 0.5;
    // float v = off_h >.0? BASE_V_VALUE: BASE_V_VALUE + off_h / (BASE_S_H - START_H_VALUE)*0.3;
    // return hsv2rgb(vec3(h,s,v));
    return hsv2rgb(vec3(gray * 0.8, 1.0, 1.0));

}

void main(){
    gl_FragColor = vec4(transfer_scheme(vTexcoord.x), 1.0);
}
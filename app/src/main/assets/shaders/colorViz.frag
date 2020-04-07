#version 300 es

//#pragma multi_compile COLOR_GRAYSCALE COLOR_HSV COLOR_BRIGHT

precision mediump float;
out vec4 gl_FragColor;
in vec2 vTexcoord;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme(float gray){
//    #ifdef COLOR_HSV
//        return hsv2rgb(vec3(gray, 1.0, 1.0));
//    #elif defined(COLOR_BRIGHT)
//        return hsv2rgb(vec3(gray * 0.5, 1.0, 1.0));
//    #endif
    return vec3(gray);
}

void main(){
    gl_FragColor = vec4(vTexcoord.x, .0,.0,1.0);//vec4(transfer_scheme(vTexcoord.x), 1.0);
}
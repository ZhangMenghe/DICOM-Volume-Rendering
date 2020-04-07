#version 300 es

precision mediump float;
uniform int uScheme;
out vec4 gl_FragColor;
in vec2 vTexcoord;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme(float gray){
    vec3 color = vec3(gray);
    if(uScheme == 1)
        color = hsv2rgb(vec3(gray, 1.0, 1.0));
    else if(uScheme == 2)
        color = hsv2rgb(vec3(gray * 180.0 / 255.0, 1.0, 1.0));
    return color;
}

void main(){
    gl_FragColor = vec4(transfer_scheme(vTexcoord.x), 1.0);
}
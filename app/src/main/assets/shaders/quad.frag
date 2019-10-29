#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec2 vTexcoord;

uniform sampler2D uSampler;

void main(){
    gl_FragColor = texture(uSampler, vTexcoord);
}

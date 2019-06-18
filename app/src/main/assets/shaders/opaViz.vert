#version 300 es
precision mediump float;
layout (location = 0) in vec3 aPosition;
uniform vec2 uOffset;
uniform vec2 uScale;
void main(){
    gl_Position = vec4(aPosition.xy * uScale + uOffset, .0, 1.0);
}
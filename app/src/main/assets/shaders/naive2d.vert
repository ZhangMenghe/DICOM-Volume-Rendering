#version 300 es

precision mediump float;
layout (location = 0) in vec2 aPosition;

void main(){
    gl_Position = vec4(aPosition.xy, .0, 1.0);
}
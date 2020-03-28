#version 300 es

precision mediump float;
layout (location = 0) in vec4 aPosition;

uniform mat4 uMVP;
void main(){
    gl_PointSize = 10.0;
    gl_Position = uMVP *vec4(aPosition.xyz, 1.0);
}
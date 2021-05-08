#version 300 es

precision mediump float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

uniform mat4 uMVP;
out vec3 vTexcoord;

void main(){
    vTexcoord = aTexCoord;
    gl_Position = uMVP * vec4(aPosition, 1.0);
}

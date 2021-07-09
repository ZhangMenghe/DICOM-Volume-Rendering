#version 300 es

precision mediump float;

layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;
out vec3 vTexcoord;

void main(){
    vTexcoord = clamp(aPosition + 0.5, vec3(.0), vec3(1.0));
    gl_Position = uMVP * vec4(aPosition, 1.0);
}

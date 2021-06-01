#version 300 es

precision mediump float;

layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform float uVolumeThickness;
out vec3 vTexcoord;

void main(){
    vTexcoord = aPosition + vec3(0.5f);
    gl_Position = uMVP * vec4(vec3(aPosition.xy, aPosition.z*uVolumeThickness), 1.0);
}

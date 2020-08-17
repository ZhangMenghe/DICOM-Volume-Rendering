#version 310 es
#extension GL_EXT_shader_io_blocks:require

precision mediump float;
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
out VS_OUT
{
	vec3 position;
	vec3 normal; // Draw with flat shading
    vec3 vbc;
} vs_out;
uniform mat4 uMVP;
uniform bool uDrawWire;
void main(){
    vec3 pos = vec3(aPos.x, -aPos.y, aPos.z);
    gl_Position = uMVP * vec4(pos,1.0);
    
    vs_out.position = pos;
    vs_out.normal = normalize(aNormal.xyz);
    if(uDrawWire){
        if(aPos.w == .0)vs_out.vbc = vec3(1.0, .0, .0);
        else if(aPos.w == 1.0)vs_out.vbc = vec3(.0, 1.0, .0);
        else vs_out.vbc = vec3(.0,.0,1.0);
    }
}
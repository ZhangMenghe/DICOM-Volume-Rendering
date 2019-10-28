#version 300 es
precision mediump float;

//layout (location = 0) in vec3 aPosition;
//layout (location = 1) in vec3 aTexCoord;
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aZposTex;//first is z pos, second is z texture pos

//layout (location = 0) in vec2 aPosition;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aZposTex;

//uniform vec3 u_step_size;
uniform mat4 uMVP;
out vec3 vTexcoord;

void main(){
    vTexcoord = vec3(aPosition.xy + 0.5, aZposTex.y);
//    vTexcoord = aTexCoord;//clamp(aTexCoord, u_step_size, 1.0 - u_step_size);
    gl_Position = uMVP * vec4(aPosition, aZposTex.x, 1.0);
//    gl_Position = vec4(aPosition.xy, 0.0, 1.0);
}

#version 300 es

#extension GL_OES_EGL_image_external_essl3 : enable
#extension GL_OES_EGL_image_external : require

precision mediump float;

out vec4 gl_FragColor;
in vec2 vTexcoord;

uniform samplerExternalOES uSampler;

void main(){
    gl_FragColor = texture(uSampler, vTexcoord);
}

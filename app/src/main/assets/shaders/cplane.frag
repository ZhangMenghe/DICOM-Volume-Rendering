#version 300 es
precision mediump float;
out vec4 gl_FragColor;
uniform vec4 uBaseColor;

void main() {
    gl_FragColor = uBaseColor / 255.0;
}

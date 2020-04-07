#version 300 es
precision mediump float;
uniform vec4 uColor;
out vec4 gl_FragColor;
void main(){
    gl_FragColor = vec4(1.0, .0,.0,1.0);//uColor;
}
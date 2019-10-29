#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform sampler3D uSampler_baked;
uniform float u_cut_texz;

void main(){
	if(vTexcoord.z > u_cut_texz) discard;
	gl_FragColor = texture(uSampler_baked, vTexcoord);
}

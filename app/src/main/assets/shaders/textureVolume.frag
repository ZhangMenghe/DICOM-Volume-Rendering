#version 300 es

precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform mediump sampler3D uSampler_baked;
uniform float u_cut_texz;
uniform bool u_front;
uniform bool u_cut;

void main(){
	if(u_cut){
		if(u_front && vTexcoord.z > u_cut_texz) discard;
		else if(!u_front && vTexcoord.z < u_cut_texz) discard;
	}
	gl_FragColor = texture(uSampler_baked, vTexcoord);
}

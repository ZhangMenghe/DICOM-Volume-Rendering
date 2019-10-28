#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

struct OpacityAdj{
	float overall;//0-1
	float lowbound; //slope adj, 0-1
	float cutoff;//0,1
};
uniform OpacityAdj uOpacitys;

uniform sampler3D uSampler_tex;
uniform sampler3D uSampler_baked;
uniform float u_cut_texz;

void main(){
	if(vTexcoord.z > u_cut_texz) discard;
	//todo: move computation to computation stage, sampling very expensive
	vec4 sampled_color = texture(uSampler_baked, vTexcoord).rgba;
	float intensity = texture(uSampler_tex, vTexcoord).r;

	float alpha = intensity * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
	alpha*=sampled_color.a;
	if(intensity < uOpacitys.cutoff) alpha = 0.0;
	gl_FragColor = vec4(sampled_color.rgb, alpha * uOpacitys.overall);
}

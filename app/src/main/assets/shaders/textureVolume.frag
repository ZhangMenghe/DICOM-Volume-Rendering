#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;
in vec2 vpos;

struct OpacityAdj{
	float overall;//0-1
	float lowbound; //slope adj, 0-1
	float cutoff;//0,1
};
uniform OpacityAdj uOpacitys;

uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;

uniform bool ub_colortrans;
void main(){
	float intensity = texture(uSampler_tex, vTexcoord).r;
	vec3 sampled_color;
	if(ub_colortrans == true)
		sampled_color = texture(uSampler_trans, vec2(intensity, 1.0)).rgb;
	else
		sampled_color = vec3(intensity);
	float alpha = intensity * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
	if(intensity< uOpacitys.cutoff) alpha = 0.0;
	gl_FragColor = vec4(sampled_color, alpha * uOpacitys.overall);
}

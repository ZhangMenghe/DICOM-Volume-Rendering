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

uniform bool ub_colortrans;
uniform bool ub_simplecube;

float START_H_VALUE = 0.1667;
float BASE_S_VALUE = 0.7;
float BASE_S_H = 0.6667;//pure blue
float BASE_V_VALUE = 0.8;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c){
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
vec3 transfer_scheme(float gray){
	// transfer a gray-scale from 0-1 to proper rgb value
	float range = 0.833;
	float h = (1.0 - START_H_VALUE) * gray + START_H_VALUE;
	float off_h = h - BASE_S_H;
	float s = off_h>.0? BASE_S_VALUE + off_h / (1.0 - BASE_S_H) * 0.3: BASE_S_VALUE + off_h / (BASE_S_H - START_H_VALUE) * 0.5;
	float v = off_h >.0? BASE_V_VALUE: BASE_V_VALUE + off_h / (BASE_S_H - START_H_VALUE)*0.3;
	return hsv2rgb(vec3(h,s,v));
}
void main(){
	if(ub_simplecube){
		gl_FragColor = vec4(vTexcoord, 1.0f);
		return;
	}
	float intensity = texture(uSampler_tex, vTexcoord).g;
	vec3 sampled_color;
	if(ub_colortrans){
//		float value =min(intensity + 0.167, 1.0);
//		gl_FragColor = vec4(hsv2rgb(vec3(value, 0.5, 0.8)), alpha * uOpacitys.overall);
		sampled_color = transfer_scheme(intensity);
	}

//		sampled_color = texture(uSampler_trans, vec2(intensity, 1.0)).rgb;
	else
		sampled_color = vec3(intensity);
	float alpha = intensity * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
	if(intensity< uOpacitys.cutoff) alpha = 0.0;
	gl_FragColor = vec4(sampled_color, alpha * uOpacitys.overall);
}

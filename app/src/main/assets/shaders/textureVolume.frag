#version 300 es
#pragma multi_compile CUTTING_PLANE

precision mediump float;

out vec4 fragColor;
in vec3 vTexcoord;

uniform mediump sampler3D uSampler_baked;
struct Plane{
	vec3 p;
	vec3 normal;
	vec3 s1, s2, s3;
};
uniform Plane uPlane;

void main(){
	#ifdef CUTTING_PLANE
		vec3 pos = vTexcoord - 0.5;
		if(dot(pos - uPlane.p, uPlane.normal) < .0) discard;
	#endif
	fragColor = texture(uSampler_baked, vTexcoord);
}
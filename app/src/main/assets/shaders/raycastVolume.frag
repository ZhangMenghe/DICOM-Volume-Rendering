#version 300 es
precision mediump float;

in vec3 frag_position; // in object space
in vec3 tex_coord;
in vec3 ray_dir;
in mat3 NormalMatrix;

uniform sampler3D uSampler_tex;
uniform bool ub_simplecube;
uniform bool ub_colortrans;
uniform vec3 uVolumeSize;

uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;
struct OpacityAdj{
    float overall;//0-1
    float lowbound; //slope adj, 0-1
    float cutoff;//0,1
};
uniform OpacityAdj uOpacitys;

out vec4 gl_FragColor;

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

void main_old(void){
    gl_FragColor = vec4(tex_coord, 1.0f);
}

void main(void){
  if(ub_simplecube){
      gl_FragColor = vec4(tex_coord, 1.0);
      return;
  }
  float sample_step = 1.0/sample_step_inverse;
  vec3 ray_pos = tex_coord; // the current ray position
  vec3 pos111 = vec3(1.0, 1.0, 1.0);
  vec3 pos000 = vec3(0.0, 0.0, 0.0);

  vec4 frag_color = vec4(0,0,0,0);
  vec4 color;
  float density, max_density = -1.0;
  vec3 best_ray_pos = ray_pos;

  do{
    ray_pos += ray_dir * sample_step;
    if (any(greaterThan(ray_pos,pos111)))
      break;
    if (any(lessThan(ray_pos,pos000)))
      break;
    density = texture(uSampler_tex, ray_pos).r;

    max_density = max(max_density, density);
    if(max_density == density)
        best_ray_pos = ray_pos;

  }while(true);

    if(max_density > -1.0){

        density = max_density;
        density += val_threshold - 0.5;
        density = density * density * density;

        float alpha = max_density * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
        if(max_density< uOpacitys.cutoff) alpha = 0.0;

        if(ub_colortrans)
            gl_FragColor = vec4(transfer_scheme(density), alpha * uOpacitys.overall);
//            gl_FragColor = vec4(texture(uSampler_trans, vec2(density, 1.0)).rgb, alpha * uOpacitys.overall);
        else
            gl_FragColor = vec4(vec3(density),alpha * uOpacitys.overall);
    }
    else
    discard;
}

#version 300 es
precision mediump float;

in vec3 frag_position; // in object space
in vec3 tex_coord;
in vec3 ray_dir;
in mat3 NormalMatrix;

uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;
uniform bool ub_simplecube;
uniform bool ub_colortrans;
uniform vec3 uVolumeSize;

uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;

out vec4 gl_FragColor;

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
        if(ub_colortrans)
            gl_FragColor = vec4(texture(uSampler_trans, vec2(density, 1.0)).rgb, 1.0);
        else
            gl_FragColor = vec4(vec3(density),1.0);
    }
    else
    discard;
}

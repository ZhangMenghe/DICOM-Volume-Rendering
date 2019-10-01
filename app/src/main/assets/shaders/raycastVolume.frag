#version 310 es

#extension GL_EXT_shader_io_blocks : require
//#define GL_FRAGMENT_PRECISION_HIGH 1
precision mediump float;

out vec4 gl_FragColor;
in VS_OUT {
    vec3 raydir;
    vec3 screenPos;
    vec3 FragPos;
    vec3 TexCoords;
} fs_in;
uniform vec3 uCamposObjSpace;

//cutting-plane
struct Plane{
    vec3 p;
    vec3 normal;
    bool upwards;
};
struct Sphere{
    vec3 center;
    float radius;
};
//uniform Plane uPlane;
uniform Sphere uSphere;

uniform mediump sampler3D uSampler_tex;
uniform bool ub_simplecube;
uniform bool ub_colortrans;
uniform bool ub_accumulate;
//uniform bool ub_viewfront;
uniform vec3 uVolumeSize;

uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;

uniform vec3 uStartPoint;
uniform float cut_percent;

struct OpacityAdj{
    float overall;//0-1
    float lowbound; //slope adj, 0-1
    float cutoff;//0,1
};
uniform OpacityAdj uOpacitys;

float START_H_VALUE = 0.1667;
float BASE_S_VALUE = 0.7;
float BASE_S_H = 0.6667;//pure blue
float BASE_V_VALUE = 0.8;

Plane uPlane;


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

vec2 RaySphere(vec3 ro, vec3 rd, vec3 center, float radius){
    vec3 oc = ro - center;
    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - 4.0*a*c;
    return discriminant < 1e-5? vec2(1e5, -1e5): vec2(-b - sqrt(discriminant) , -b + sqrt(discriminant)) / (2.0*a);
}
vec2 RayCube(vec3 ro, vec3 rd, vec3 extents) {
    vec3 tMin = (-extents - ro) / rd;
    vec3 tMax = (extents - ro) / rd;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    return vec2(max(max(t1.x, t1.y), t1.z), min(min(t2.x, t2.y), t2.z));
}
float RayPlane(vec3 ro, vec3 rd, vec3 planep, vec3 planen) {
    float d = dot(planen, rd);
    float t = dot(planep - ro, planen);
    return d > 1e-5 ? (t / d) : (t > .0 ? 1e5 : -1e5);
}


void getCuttingPlane(vec3 rd){
    uPlane.p = uStartPoint + normalize(rd) * cut_percent * 1.75;
    uPlane.upwards = true;
    uPlane.normal = rd;
}

vec4 Sample(vec3 p){
    vec4 color;
    float intensity = texture(uSampler_tex, p).r;
    intensity += val_threshold - 0.5;
    intensity = clamp(intensity * brightness / 250.0, 0.0, 1.0);

    color.rgb = vec3(intensity);
    color.a = (dot((p - .5) - uPlane.p, uPlane.normal) < .0) ? .0 : color.r;
    return color;
}
vec4 Volume(float head, float tail){
    vec3 ro = uCamposObjSpace + 0.5;
    vec3 rd = normalize(fs_in.raydir);
    vec4 sum = vec4(.0);
    int steps = 0;

    float vmax = .0;
    for(float t = head; t<tail; ){
        if(sum.a >= 0.95) break;
        vec3 p = ro + rd * t;
        vec4 val_color = Sample(p);
        if(ub_accumulate){
            if(val_color.a > 0.01){
                sum.rgb += (1.0 - sum.a) *  val_color.a* val_color.rgb;
                sum.a += (1.0 - sum.a) * val_color.a;
            }
            if(val_color.r > vmax) vmax = val_color.r;
        }else if(val_color.r > vmax){
             vmax = val_color.r;
        }
        t+=sample_step_inverse;//val_color.a > 0.01? sample_step_inverse: sample_step_inverse * 4.0;
        steps++;
    }
    vec3 sample_color;
    if(ub_accumulate){
        if(ub_colortrans)
            return vec4(transfer_scheme(sum.r), clamp(sum.a, 0.0, 1.0));
        else
            return vec4(sum.rgb, clamp(sum.a, 0.0, 1.0));
    }else{
        if(ub_colortrans)
            return vec4(transfer_scheme(vmax), 1.0);
        else
            return vec4(vec3(vmax), 1.0);
    }
//    float alpha = sum.a * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
//    if(sum.r< uOpacitys.cutoff) alpha = 0.0;

//    return vec4(sum.rgb, alpha * uOpacitys.overall);
}
void main(void){
    vec3 ray_origin = uCamposObjSpace;
    vec3 ray_dir = normalize(fs_in.raydir);

    vec2 intersect = RayCube(ray_origin, ray_dir, vec3(0.5));
    intersect.x = max(.0, intersect.x);
    if(cut_percent != .0){
        //Ray-plane
        getCuttingPlane(-uCamposObjSpace);

        if(uPlane.upwards)//要上面
            intersect.x = max(intersect.x, RayPlane(ray_origin, ray_dir, uPlane.p, uPlane.normal));
        else//要下面
            intersect.y = min(RayPlane(ray_origin, ray_dir, uPlane.p, uPlane.normal), intersect.y);
    }

    //Ray-Sphere
    if(dot(uSphere.center, uCamposObjSpace) > .0){
        vec2 sphere_limit = RaySphere(ray_origin, ray_dir,uSphere.center, uSphere.radius);
        if(sphere_limit.x < sphere_limit.y){
            intersect.x = max(intersect.x, sphere_limit.y);
        }
    }

    if (intersect.y < intersect.x)
        discard;
    else
        gl_FragColor = ub_simplecube? vec4(0.8, 0.8, .0, 1.0) : Volume(intersect.x, intersect.y);
}

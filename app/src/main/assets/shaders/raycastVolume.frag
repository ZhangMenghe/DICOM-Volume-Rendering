#version 310 es

#extension GL_EXT_shader_io_blocks : require

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
};
//struct Sphere{
//    vec3 center;
//    float radius;
//};
uniform Plane uPlane;
//uniform Sphere uSphere;

uniform mediump sampler3D uSampler_tex;
//uniform bool ub_accumulate;//
uniform bool ub_cuttingplane;

uniform float sample_step_inverse;      // step used to advance the sampling ray


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

vec4 Sample(vec3 p){
    return texture(uSampler_tex, p);
}
vec4 subDivide(vec3 p, vec3 ro, vec3 rd, float t, float StepSize){
    float t0 = t - StepSize * 4.0;
    float t1 = t;
    float tm;

    #define BINARY_SUBDIV tm = (t0 + t1) * .5; p = ro + rd * tm; if (Sample(p).a > .01) t1 = tm; else t0 = tm;
    BINARY_SUBDIV
    BINARY_SUBDIV
    BINARY_SUBDIV
    BINARY_SUBDIV
    #undef BINARY_SUBDIV

    t = tm;
    return Sample(p);
}
vec4 Volume(float head, float tail){
    vec3 ro = uCamposObjSpace + 0.5;
    vec3 rd = normalize(fs_in.raydir);
    vec4 sum = vec4(.0);
    int steps = 0; float pd = .0;

    float vmax = .0;
    for(float t = head; t<tail; ){
        if(sum.a >= 0.95) break;
        vec3 p = ro + rd * t;
        vec4 val_color = Sample(p);
//        if(ub_accumulate){
            if(val_color.a > 0.01){
                if(pd < 0.01) val_color = subDivide(p, ro, rd, t, sample_step_inverse);
                sum.rgb += (1.0 - sum.a) *  val_color.a* val_color.rgb;
                sum.a += (1.0 - sum.a) * val_color.a;
            }
//        }else if(val_color.r > vmax){
//             vmax = max(val_color.r, vmax);
//        }
        t += val_color.a > 0.01? sample_step_inverse: sample_step_inverse * 4.0;
        steps++;
        pd = sum.a;
    }
//    if(ub_accumulate){
        return vec4(sum.rgb, clamp(sum.a, 0.0, 1.0));
//    }else{
//        return vec4(vec3(vmax), 1.0);
//    }
//    float alpha = sum.a * (1.0 - uOpacitys.lowbound) + uOpacitys.lowbound;
//    if(sum.r< uOpacitys.cutoff) alpha = 0.0;

//    return vec4(sum.rgb, alpha * uOpacitys.overall);
}
void main_old(void){
    gl_FragColor = texture(uSampler_tex, fs_in.TexCoords);
}
void main(void){
    vec3 ray_origin = uCamposObjSpace;
    vec3 ray_dir = normalize(fs_in.raydir);

    vec2 intersect = RayCube(ray_origin, ray_dir, vec3(0.5));
    intersect.x = max(.0, intersect.x);
    if(ub_cuttingplane){
        //Ray-plane
        if(dot(uPlane.normal, -uCamposObjSpace) > .0) //plane_n = -plane_n;
            intersect.x = max(intersect.x, RayPlane(ray_origin, ray_dir, uPlane.p, uPlane.normal ));
        else
            intersect.y = min(RayPlane(ray_origin, ray_dir, uPlane.p, -uPlane.normal), intersect.y);
    }

    //Ray-Sphere

//    vec2 sphere_limit = RaySphere(ray_origin, ray_dir,uSphere.center, uSphere.radius);
//
//    if(sphere_limit.x < sphere_limit.y){
//        if(dot(uSphere.center, uCamposObjSpace) > .0)
//        intersect.x = max(intersect.x, sphere_limit.y);
//        else
//        intersect.y = min(intersect.y, sphere_limit.x);
//    }
//
    if (intersect.y < intersect.x)
        discard;
    gl_FragColor = Volume(intersect.x, intersect.y);
}

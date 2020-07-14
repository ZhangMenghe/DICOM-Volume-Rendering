#version 310 es
#pragma multi_compile CUTTING_PLANE

#extension GL_EXT_shader_io_blocks : require

precision mediump float;

out vec4 fragColor;
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
uniform Plane uPlane;
uniform mediump sampler3D uSampler;
uniform float sample_step_inverse;

vec2 RaySphere(vec3 ro, vec3 rd, vec3 center, float radius){
    //Ray-Sphere

    //    vec2 sphere_limit = RaySphere(ray_origin, ray_dir,uSphere.center, uSphere.radius);
    //
    //    if(sphere_limit.x < sphere_limit.y){
    //        if(dot(uSphere.center, uCamposObjSpace) > .0)
    //        intersect.x = max(intersect.x, sphere_limit.y);
    //        else
    //        intersect.y = min(intersect.y, sphere_limit.x);
    //    }
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
    return texture(uSampler, p);
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
vec4 Volume(vec3 ro, vec3 rd, float head, float tail){
    vec4 sum = vec4(.0);
    int steps = 0; float pd = .0;

    float vmax = .0;
    for(float t = head; t<tail; ){
        if(sum.a >= 0.95) break;
        vec3 p = ro + rd * t;
        vec4 val_color = Sample(p);
        if(val_color.a > 0.01){
            if(pd < 0.01) val_color = subDivide(p, ro, rd, t, sample_step_inverse);
            sum.rgb += (1.0 - sum.a) *  val_color.a* val_color.rgb;
            sum.a += (1.0 - sum.a) * val_color.a;
        }
        t += val_color.a > 0.01? sample_step_inverse: sample_step_inverse * 4.0;
        steps++;
        pd = sum.a;
    }
    return vec4(sum.rgb, clamp(sum.a, 0.0, 1.0));
}
//void main(void){
//    fragColor =  vec4(fs_in.TexCoords, 1.0);
//}
void main(void){
    vec3 ray_origin = uCamposObjSpace;
    vec3 ray_dir = normalize(fs_in.raydir);

    vec2 intersect = RayCube(ray_origin, ray_dir, vec3(0.5));
    intersect.x = max(.0, intersect.x);
    #ifdef CUTTING_PLANE
        //Ray-plane
        if(dot(uPlane.normal, -uCamposObjSpace) > .0)
            intersect.x = max(intersect.x, RayPlane(ray_origin, ray_dir, uPlane.p, uPlane.normal ));
        else
            intersect.y = min(RayPlane(ray_origin, ray_dir, uPlane.p, -uPlane.normal), intersect.y);
    #endif

    if (intersect.y < intersect.x)
        discard;
    fragColor = Volume(ray_origin+0.5, ray_dir, intersect.x, intersect.y);
}

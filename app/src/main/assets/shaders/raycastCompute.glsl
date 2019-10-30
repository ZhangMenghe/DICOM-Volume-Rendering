#version 310 es

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image2D destTex;

uniform vec2 u_con_size;
uniform float u_fov;

uniform mat4 u_WorldToModel;
uniform mat4 u_CamToWorld;

uniform vec3 uCamposObjSpace;
uniform float uViewDir;
uniform float usample_step_inverse;

const float constantNCP = 1.0;
vec3 VolumeSize;

vec2 RayCube(vec3 ro, vec3 rd, vec3 extents) {
    vec3 tMin = (-extents - ro) / rd;
    vec3 tMax = (extents - ro) / rd;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    return vec2(max(max(t1.x, t1.y), t1.z), min(min(t2.x, t2.y), t2.z));
}
vec4 Sample(vec3 p){
    vec3 coord = clamp(p, vec3(usample_step_inverse), vec3(1.0-usample_step_inverse));
    return imageLoad(srcTex, ivec3(VolumeSize *coord));
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
//            if(pd < 0.01) val_color = subDivide(p, ro, rd, t, usample_step_inverse);
            sum.rgb += (1.0 - sum.a) *  val_color.a* val_color.rgb;
            sum.a += (1.0 - sum.a) * val_color.a;
        }
        t += val_color.a > 0.01? usample_step_inverse: usample_step_inverse * 4.0;
        steps++;
        pd = sum.a;
    }
    return vec4(sum.rgb, clamp(sum.a, 0.0, 1.0));
}

vec4 tracing(float u, float v){
    float tangent = tan(u_fov / 2.0); // angle in radians
    float ar = (float(u_con_size.x) / u_con_size.y);

    vec3 ro = uCamposObjSpace;
    vec3 rd = vec3(normalize(u_WorldToModel * u_CamToWorld *vec4(u* tangent*ar, v*tangent, -constantNCP, .0)));

    vec2 intersect = RayCube(ro, rd, vec3(0.5));
    intersect.x = max(.0, intersect.x);
    if(intersect.y < intersect.x) return vec4(.0);

    VolumeSize = vec3(imageSize(srcTex));
    return Volume(ro + 0.5, rd, intersect.x, intersect.y);
}
void main() {
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    float cx = float(storePos.x); float cy = float(storePos.y);
    if ( cx >= u_con_size.x ||  cy >= u_con_size.y) return;
    cy = u_con_size.y - cy;
    vec2 uv = (vec2(cx, cy) + 0.5) / u_con_size * 2.0 - 1.0;
    imageStore(destTex, ivec2(cx, cy), tracing(uv.x, uv.y));
}

#version 310 es

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(binding = 0, rgba8)readonly uniform mediump image3D srcTex;
layout(binding = 1, rgba8)writeonly uniform mediump image2D destTex;

uniform vec2 u_con_size;

void main() {
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    if (float(storePos.x) >= u_con_size.x || float(storePos.y) >= u_con_size.y) {
        return;
    }
//    vec2 pos = vec2(pix) / vec2(size.x, size.y);
//    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
//    vec4 color = trace(eye, dir);
    imageStore(destTex, storePos, vec4(0.8,0.8,0.0,1.0));
}

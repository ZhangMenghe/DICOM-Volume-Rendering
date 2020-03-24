#version 310 es
#extension GL_EXT_shader_io_blocks : require

precision mediump float;
out vec4 gl_FragColor;

in VS_OUT {
    float v_alpha;
    vec2 v_texcoord;
} fs_in;
uniform sampler2D uSampler;
uniform vec3 uColor;
void main() {
    float r = texture(uSampler, fs_in.v_texcoord).r;
    gl_FragColor = vec4(uColor.rgb, r * fs_in.v_alpha);
}

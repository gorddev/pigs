#version 410 core
precision mediump float;

in vec2 vUV;
in vec3 vNorm;

out vec4 color;

uniform vec2 myvec;
uniform sampler2D uTex;

void main()
{
    vec2 animatedUV = vUV + vec2(sin(pg_time/1000.0), cos(pg_time/1000.0)) * 0.1;
    color = texture(uTex, animatedUV);
}

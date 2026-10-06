#version 410 core
precision mediump float;

uniform vec2 myvec;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNorm;

out vec2 vUV;
out vec3 vNorm;

void main() {
    vUV = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

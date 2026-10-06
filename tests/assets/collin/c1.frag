#version 410 core

in vec2 vUV;

out vec4 color;

uniform sampler2D uTex;

void main() {
  vec4 texCol = texture(uTex, vUV);

  switch (int(texCol.r*255.0)) {
  case 0:
    color = vec4(0.5, 0.0, 0.0, 0.5);
  break;
  case 1:
    color = vec4(0.0, 0.5, 0.0, 0.5);
  break;
  case 2:
    color = vec4(0.0, 0.0, 0.5, 0.5);
  break;
  case 3:
    color = vec4(0.5, 0.0, 0.5, 0.5);
  break;
  case 4:
    color = vec4(0.0, 0.5, 0.5, 0.5);
  break;
  default:
    color = vec4(0.0);
  break;
  }
}

#version 440

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aUv;

out vec2 uv;

void main() {
    uv = aUv;
    gl_Position = vec4(aPos, 1);
}

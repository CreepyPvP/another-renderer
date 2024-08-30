#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aUv;

out vec3 color;
out vec3 normal;
out vec2 uv;

uniform mat4 proj;
uniform mat4 model;

void main() {
    color = aColor;
    normal = normalize((model * vec4(aNorm, 0)).xyz);
    uv = aUv;

    gl_Position = proj * model * vec4(aPos, 1);
}

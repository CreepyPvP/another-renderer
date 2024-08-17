#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aUv;

out vec3 color;
out vec3 normal;
out vec2 uv;

uniform mat4 proj;

void main() {
    color = aColor;
    normal = aNorm;
    uv = aUv;

    gl_Position = proj * vec4(aPos, 1);
}

#version 440

layout(location = 0) in vec3 aPos;

uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * model * vec4(aPos, 1);
}

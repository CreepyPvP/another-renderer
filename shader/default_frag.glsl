#version 440

in vec3 color;
in vec3 normal;
in vec2 uv;

out vec4 out_Color;

const vec3 l = normalize(vec3(1, 3, -2));

void main()
{
    out_Color = vec4(color * max(0.8 * dot(normal, l), 0) + vec3(0.2) * color, 1);
    // out_Color = vec4(normal, 1);
}

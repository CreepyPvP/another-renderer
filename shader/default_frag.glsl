#version 440

in vec3 color;
in vec3 normal;
in vec2 uv;

uniform sampler2D diffuse_texture;

out vec4 out_Color;

const vec3 l = normalize(vec3(1, 3, -2));

void main()
{
    vec3 diffuse_color = texture(diffuse_texture, uv).rgb * color;

    out_Color = vec4(diffuse_color * max(0.8 * dot(normal, l), 0) + vec3(0.2) * diffuse_color, 1);
    // out_Color = vec4(normal, 1);
    // out_Color = vec4(uv, 0, 1);
}

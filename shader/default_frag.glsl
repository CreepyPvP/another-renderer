#version 440

#define PI 3.1415926

in vec3 color;
in vec3 normal;
in vec3 world_pos;
in vec2 uv;

uniform vec3 camera_pos;
uniform sampler2D diffuse_texture;

// .x metallic, .y roughness
uniform vec3 pbr;
uniform vec3 base_color;

out vec4 out_Color;

const vec3 l = normalize(vec3(1, 3, -2));

float g_1(vec3 n, vec3 v, float roughness)
{
    float dot_n_v = dot(n, v);
    float k = (roughness + 1) * (roughness + 1) / 8;
    return dot_n_v / (dot_n_v * (1 - k) + k);
}

void main()
{
    float metallic = pbr.x;
    float roughness = pbr.y;

    vec3 color_sample = texture(diffuse_texture, uv).rgb * color * base_color;

    vec3 diffuse_color = (1.0 - metallic) * color_sample;
    // TODO: Add specular reflective term
    vec3 specular_color = metallic * color_sample;

    vec3 n = normalize(normal);
    vec3 v = normalize(camera_pos - world_pos);
    vec3 h = normalize(v + l);

    roughness = clamp(roughness, 0.02, 1);
    float a = roughness * roughness;

    float dot_n_h = dot(n, h);
    float dot_v_h = dot(v, h);
    float dot_n_l = dot(n, l);
    float dot_n_v = dot(n, v);

    float tmp = ((dot_n_h * dot_n_h) * (a * a - 1) + 1);
    float ndf = a / (PI * tmp * tmp);

    float shadowing = g_1(n, l, roughness) * g_1(n, v, roughness);

    vec3 fresnel = specular_color + (vec3(1) - specular_color) * pow(2, (-5.55473 * dot_v_h - 6.98316) * dot_v_h);

    vec3 specular = ndf * fresnel * shadowing / (4 * dot_n_l * dot_n_v);
    vec3 diffuse = diffuse_color / PI;

    vec3 total_color = specular + diffuse;

    float dir = max(0, dot(n, l));
    out_Color = vec4(total_color * dir + color_sample * 0.1, 1);
    // out_Color = vec4(metallic, roughness, 0, 1);
    // out_Color = vec4(total_color * max(0.8 * dot(n, l), 0) + vec3(0.2) * total_color, 1);
    // out_Color = vec4(n, 1);
    // out_Color = vec4(uv, 0, 1);
}

#version 440

in vec2 uv;

uniform sampler2D frame;

out vec4 out_Color;

void main()
{
    vec3 final_color = texture(frame, uv).rgb;

    final_color = pow(final_color, vec3(1.0 / 2.2));

    out_Color = vec4(final_color, 1);
}

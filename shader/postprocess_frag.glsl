#version 440

in vec2 uv;

uniform sampler2D frame;

out vec4 out_Color;

vec3 aces(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0, 1);
}

void main()
{
    vec3 frame_sample = texture(frame, uv).rgb;

    vec3 tonemapped = aces(frame_sample);
    // vec3 tonemapped = frame_sample;

    vec3 final_color = pow(tonemapped, vec3(1.0 / 2.2));

    out_Color = vec4(final_color, 1);
}

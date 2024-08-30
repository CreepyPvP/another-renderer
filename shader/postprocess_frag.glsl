#version 440

in vec2 uv;

uniform sampler2D frame;

out vec4 out_Color;

void main()
{
    out_Color = texture(frame, uv);
    out_Color.rgb = vec3(1) - out_Color.rgb;
}

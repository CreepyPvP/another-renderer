#version 440

in vec3 color;
in vec2 uv;

out vec4 out_Color;

void main()
{
    out_Color = vec4(color, 1);
}

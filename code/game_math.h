#pragma once

#include "game.h"

#define PI 3.14159

struct Mat4
{
    f32 v[16];
    
    Mat4 operator*(Mat4 b);
};

struct V2
{
    f32 x;
    f32 y;
};

union V3
{
    struct {
        f32 x;
        f32 y;
        f32 z;
    };

    struct {
        f32 r;
        f32 g;
        f32 b;
    };

    V3 operator-(V3 b);
    V3 operator+(V3 b);
};

inline V2 v2(f32 x)
{
    return { x, x };
}

inline V2 v2(f32 x, f32 y)
{
    return { x, y };
}

inline V3 v3(f32 x)
{
    return { x, x, x };
}

inline V3 v3(f32 x, f32 y, f32 z)
{
    return { x, y, z };
}

inline Mat4 mat4(f32 x)
{
    return { x, 0, 0, 0,
             0, x, 0, 0,
             0, 0, x, 0,
             0, 0, 0, x };
}

inline f32 radians(f32 degrees)
{
    return degrees / 360 * 2 * PI;
}

inline V3 radians(V3 degrees)
{
    return v3(radians(degrees.x), radians(degrees.y), radians(degrees.z));
}

V2 norm(V2 a);

V3 norm(V3 a);
f32 dot(V3 a, V3 b);
V3 cross(V3 a, V3 b);

Mat4 perspective(f32 fov, f32 aspect, f32 near_plane, f32 far_plane);
Mat4 ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f);
Mat4 look_at(V3 eye, V3 target, V3 up);

Mat4 mat_translate(V3 value);
Mat4 mat_scale(V3 value);
Mat4 mat_rotate(V3 value);

#pragma once

#include "game.h"

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
};

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

V3 norm(V3 a);
f32 dot(V3 a, V3 b);
V3 cross(V3 a, V3 b);

Mat4 perspective(f32 fov, f32 aspect, f32 near_plane, f32 far_plane);
Mat4 look_at(V3 eye, V3 target, V3 up);

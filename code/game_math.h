#pragma once

#include "game.h"

struct Mat4
{
    f32 value[16];
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
};

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

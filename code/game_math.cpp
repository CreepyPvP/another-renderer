#include "game_math.h"

#include <math.h>

Mat4 Mat4::operator*(Mat4 b)
{
    Mat4 res;

    for (u32 i = 0; i < 4; ++i) {
        for (u32 j = 0; j < 4; ++j) {
            float acc = 0;
            for (u32 k = 0; k < 4; ++k) {
                acc += v[i + 4 * k] * b.v[4 * j + k];
            }
            res.v[i + 4 * j] = acc;
        }
    }

    return res;
}

V2 norm(V2 a)
{
    f32 len = sqrt(a.x * a.x + a.y * a.y );
    if (len < 0.0001) 
    {
        return v2(0);
    }
    return v2(a.x / len, a.y / len);
}

V3 V3::operator-(V3 b)
{
    return { x - b.x, y - b.y, z - b.z };
}

V3 V3::operator+(V3 b)
{
    return { x + b.x, y + b.y, z + b.z };
}

V3 norm(V3 a)
{
    f32 len = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    if (len < 0.0001) 
    {
        return v3(0);
    }
    return v3(a.x / len, a.y / len, a.z / len);
}

f32 dot(V3 a, V3 b) 
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

V3 cross(V3 a, V3 b)
{
    V3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Mat4 perspective(f32 fov, f32 aspect, f32 near_plane, f32 far_plane)
{
    Mat4 res = {};

    float tan_fov = tan(fov / 2);
    res.v[0] = 1.0f / (tan_fov * aspect);
    res.v[5] = 1.0f / (tan_fov);
    res.v[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    res.v[11] = -1;
    res.v[14] = -(2 * near_plane * far_plane) / (far_plane - near_plane);

    return res;
}

Mat4 ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    return {
        2/(r-l), 0, 0, 0,
        0, 2/(t-b), 0, 0,
        0, 0, -2/(f-n), 0,
        -(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1,
    };
}

Mat4 look_at(V3 eye, V3 target, V3 up)
{
    Mat4 res = {};

    V3 f = norm(target - eye);
    V3 s = norm(cross(f, up));
    V3 u = cross(s, f);

    res.v[0] = s.x;
    res.v[4] = s.y;
    res.v[8] = s.z;

    res.v[1] = u.x;
    res.v[5] = u.y;
    res.v[9] = u.z;

    res.v[2] = -f.x;
    res.v[6] = -f.y;
    res.v[10] = -f.z;

    res.v[12] = -dot(s, eye);
    res.v[13] = -dot(u, eye);
    res.v[14] = dot(f, eye);

    res.v[15] = 1;

    return res;
}

Mat4 mat_translate(V3 value)
{
    return {
        1,       0,       0,       0,
        0,       1,       0,       0,
        0,       0,       1,       0,
        value.x, value.y, value.z, 1,
    };
}

Mat4 mat_scale(V3 value)
{
    return {
        value.x, 0, 0, 0,
        0, value.y, 0, 0,
        0, 0, value.z, 0,
        0, 0, 0, 1,
    };
}

Mat4 mat_rotate(V3 value)
{
    f32 cosx = cos(value.x);
    f32 sinx = sin(value.x);
    f32 cosy = cos(value.y);
    f32 siny = sin(value.y);
    f32 cosz = cos(value.z);
    f32 sinz = sin(value.z);

    Mat4 rx = {
        1, 0, 0, 0,
        0, cosx, sinx, 0,
        0, -sinx, cosx, 0,
        0, 0, 0, 1,
    };

    Mat4 ry = {
        cosy, 0, -siny, 0,
        0, 1, 0, 0,
        siny, 0, cosy, 0,
        0, 0, 0, 1,
    };

    Mat4 rz = {
        cosz, sinz, 0, 0,
        -sinz, cosz, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    return rx * ry * rz;
}

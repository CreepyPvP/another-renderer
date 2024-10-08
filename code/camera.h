#pragma once

#include "game.h"
#include "game_math.h"


struct Camera
{
    V3 pos;
    V3 front;

    f32 yaw;
    f32 pitch;
};

void init_camera(Camera *camera, V3 pos, V3 front);
void update_camera(Camera *camera, u8 buttons_pressed, f32 speed, f32 delta);
void update_camera_mouse(Camera *camera, V2 mouse_input);

Mat4 to_view_matrix(Camera *camera);

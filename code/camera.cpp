#include "camera.h"

#include <math.h>


void init_camera(Camera *camera, V3 pos, V3 front)
{
    camera->pos = pos;
    camera->front = norm(front);
    camera->speed = 3;

    // TODO: Calculate pitch and yaw here to prevent jumping
    camera->yaw = 0;
    camera->pitch = 0;
}

void update_camera(Camera *camera, u8 buttons_pressed, float delta)
{
    V2 movement = v2(0);

    // w
    if (buttons_pressed & (1 << 0)) {
        movement.x += 1;
    }
    // s
    if (buttons_pressed & (1 << 1)) {
        movement.x -= 1;
    }
    // a
    if (buttons_pressed & (1 << 2)) {
        movement.y -= 1;
    }
    // d
    if (buttons_pressed & (1 << 3)) {
        movement.y += 1;
    }

    movement = norm(movement);

    V3 right = norm(cross(camera->front, v3(0, 1, 0)));

    f32 speed = camera->speed;

    camera->pos.x += camera->front.x * movement.x * delta * speed;
    camera->pos.y += camera->front.y * movement.x * delta * speed;
    camera->pos.z += camera->front.z * movement.x * delta * speed;
    camera->pos.x += right.x * movement.y * delta * speed;
    camera->pos.y += right.y * movement.y * delta * speed;
    camera->pos.z += right.z * movement.y * delta * speed;
};

void update_camera_mouse(Camera *camera, V2 mouse_input)
{
    camera->yaw += mouse_input.x;
    camera->pitch -= mouse_input.y;

    if (camera->pitch > 89.0) {
        camera->pitch = 89.0;
    } else if (camera->pitch < -89.0) {
        camera->pitch = -89.0;
    }

    f32 pitch = camera->pitch;
    f32 yaw = camera->yaw;

    V3 dir;
    dir.x = cos(radians(yaw)) * cos(radians(pitch));
    dir.y = sin(radians(pitch));
    dir.z = sin(radians(yaw)) * cos(radians(pitch));
    camera->front = norm(dir);
}

Mat4 to_view_matrix(Camera *camera)
{
    return look_at(camera->pos, camera->pos + camera->front, v3(0, 1, 0));
}

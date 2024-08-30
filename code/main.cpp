#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "game_math.h"
#include "renderer.h"
#include "camera.h"

u32 window_width = 960;
u32 window_height = 540;
bool window_active = true;

bool fullscreen = false;

f64 prev_mouse_x;
f64 prev_mouse_y;

GLFWwindow *window;

Camera camera;

Framebuffer main_target = {};
Framebuffer postprocess_target = {};

void update_framebuffer()
{
    opengl_destroy_framebuffer(&main_target);
    main_target = opengl_create_framebuffer(window_width, window_height, FRAMEBUFFER_COLOR | FRAMEBUFFER_DEPTH  | FRAMEBUFFER_MULTISAMPLED);
    postprocess_target = opengl_create_framebuffer(window_width, window_height, FRAMEBUFFER_COLOR);
    // main_target = opengl_create_framebuffer(window_width, window_height, FRAMEBUFFER_MULTISAMPLED | FRAMEBUFFER_COLOR | FRAMEBUFFER_DEPTH);
}

void resize_callback(GLFWwindow *window, i32 width, i32 height)
{
    window_width = width;
    window_height = height;

    window_active = width && height;
}

void mouse_callback(GLFWwindow *window, f64 pos_x, f64 pos_y)
{
    V2 diff = v2((pos_x - prev_mouse_x) * 0.1, (pos_y - prev_mouse_y) * 0.1);

    prev_mouse_x = pos_x;
    prev_mouse_y = pos_y;

    update_camera_mouse(&camera, diff);
}

void create_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    
    GLFWmonitor *monitor = NULL;
    if (fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    window = glfwCreateWindow(window_width, window_height, "Game", monitor, NULL);

    assert(window);

    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
}

i32 main()
{
    assert(sizeof(size_t) >= sizeof(u32));
    assert(sizeof(char) == sizeof(u8));

    create_window();

    opengl_initialize();
    // update_framebuffer();

    CommandBuffer commands;
    Mat4 projection;
    Model bunny = parse_obj("assets", "bunny.obj");
    // Model sponza = parse_obj("assets/sponza", "sponza.obj");
    Shader post_shader = opengl_load_shader("shader/postprocess_vert.glsl", "shader/postprocess_frag.glsl");

    init_camera(&camera, v3(0, 0, 3), v3(0, 0, -1));

    while (!glfwWindowShouldClose(window))
    {
        if (!window_active)
        {
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (main_target.width != window_width || main_target.height != window_height)
        {
            update_framebuffer();
            projection = perspective(45, (f32) window_width / (f32) window_height, 0.2, 10000);
        }

        u8 input = 0;

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            input |= (1 << 0);
        }

        if (glfwGetKey(window, GLFW_KEY_S))
        {
            input |= (1 << 1);
        }

        if (glfwGetKey(window, GLFW_KEY_A))
        {
            input |= (1 << 2);
        }

        if (glfwGetKey(window, GLFW_KEY_D))
        {
            input |= (1 << 3);
        }

        f32 delta = 1.0f / 60.0f;
        command_buffer(&commands);

        update_camera(&camera, input, delta);

        commands.group->proj = projection * to_view_matrix(&camera);

        set_render_target(&main_target);
        push_clear({0.4, 0.2, 0.3, 1.0});
        // push_draw_model(sponza);

        static f32 degree;
        degree += delta * 90;
        push_draw_model(bunny, v3(0, -5, 0), radians(v3(degree, 0, 0)), v3(40));
        push_draw_model(bunny, v3(80, -5, 0), radians(v3(0, degree, 0)), v3(40));
        push_draw_model(bunny, v3(160, -5, 0), radians(v3(0, 0, degree)), v3(40));

        // for (i32 x = -1; x <= 1; ++x)
        // {
        //     for (i32 y = -1; y <= 1; ++y)
        //     {
        //         push_draw_model(bunny, {3.0f * x, 3.0f * y, 0});
        //     }
        // }

        push_blit(&postprocess_target, &main_target);

        set_render_target(NULL);
        push_screen_rect(postprocess_target.color, &post_shader);

        opengl_execute_commands(&commands, window_width, window_height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

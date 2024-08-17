#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "renderer.h"

u32 window_width = 960;
u32 window_height = 540;
bool fullscreen = false;

GLFWwindow *window;

void resize_callback(GLFWwindow *window, i32 width, i32 height)
{
    window_width = width;
    window_height = height;
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
    // glfwSetCursorPosCallback(global_window.handle, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
}

i32 main()
{
    assert(sizeof(size_t) >= sizeof(u32));
    assert(sizeof(char) == sizeof(u8));

    create_window();

    initialize_backend();

    CommandBuffer commands;
    Model bunny = parse_obj("assets/bunny.obj");

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        {
            glfwSetWindowShouldClose(window, true);
        }

        command_buffer(&commands);

        commands.group->proj = mat4(1);

        push_clear({0.1, 0.1, 0.2, 1.0});
        push_draw_model(bunny);

        execute_commands(&commands, window_width, window_height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

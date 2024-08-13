#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void initialize_backend()
{
    assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
}

void handle_clear_command(ClearCommand *clear)
{
    Color color = clear->color;
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void execute_commands(CommandBuffer *commands, u32 width, u32 height)
{
    glViewport(0, 0, width, height);

    u32 offset = 0;
    while (offset < commands->write)
    {
        CommandType *type = (CommandType *) advance_pointer(commands->memory, offset);

        switch (*type)
        {
            case Command_Clear: {
                ClearCommand *clear = (ClearCommand *) advance_pointer(commands->memory, offset);
                handle_clear_command(clear);
                offset += sizeof(ClearCommand);
                break;
            }

            default: {
                assert(0);
            }
        }
    }
}

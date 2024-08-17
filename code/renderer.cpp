#include "renderer.h"
#include "tinyobj_loader_c.h"

CommandBuffer *active_commands;

void command_buffer(CommandBuffer *commands)
{
    *commands = {};
    active_commands = commands;

    push_render_group();
}

#define push_command(type) ((type *) _push_command(sizeof(type)))

void *_push_command(u32 size)
{
    u32 write = active_commands->write;
    assert(write + size <= sizeof(active_commands->memory));
    void *command = &active_commands->memory[write];
    active_commands->write += size;
    return command;
}

RenderGroup *push_render_group()
{
    assert(active_commands->render_group_count < 10);
    RenderGroup *group = &active_commands->group[active_commands->render_group_count];
    active_commands->render_group_count++;
    return group;
}

void push_clear(Color color)
{
    ClearCommand *clear = push_command(ClearCommand);
    clear->type = Command_Clear;
    clear->color = color;
}

void push_draw_model(Model model, u32 group)
{
    DrawModelCommand *draw = push_command(DrawModelCommand);
    draw->type = Command_DrawModel;
    draw->model = model;
    draw->group = group;
}

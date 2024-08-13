#include "renderer.h"
#include "tinyobj_loader_c.h"

CommandBuffer *active_commands;

void set_active_commands(CommandBuffer *commands)
{
    active_commands = commands;
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

void push_clear_command(Color color)
{
    ClearCommand *clear = push_command(ClearCommand);
    clear->type = Command_Clear;
    clear->color = color;
}

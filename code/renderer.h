#pragma once

#include "game.h"

struct Color
{
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct Mesh
{
};

struct Model
{
    u32 mesh_count;
    Mesh *meshes;
};

struct CommandBuffer
{
    u32 write;
    u8 memory[KiloBytes(32)];
}; 

enum CommandType
{
    Command_Clear,
};

void set_active_commands(CommandBuffer *commands);

struct ClearCommand
{
    CommandType type;
    Color color;
};

void push_clear_command(Color color);

// Backend...

void initialize_backend();

void execute_commands(CommandBuffer *commands, u32 width, u32 height);


#pragma once

#include "game.h"
#include "game_math.h"


struct Color
{
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct Vertex
{
    V3 position;
    V3 normal;
    V3 color;
    V2 uv;
};

struct Texture
{
    u32 id;
};

enum
{
    Material_DiffuseTexture = (1 << 0),
};

struct Material
{
    u32 flags;
    Texture diffuse;
};

struct Mesh
{
    u32 vertex_count;
    u32 vertex_offset;

    // -1 means default material
    i32 material_index;
};

struct Model
{
    u32 id;
    u32 total_vertices;

    u32 mesh_count;
    Mesh *meshes;

    u32 material_count;
    Material *materials;
};

struct RenderGroup
{
    Mat4 proj;
};

struct CommandBuffer
{
    u32 write;
    u8 memory[KiloBytes(32)];

    u32 render_group_count;
    RenderGroup group[10];
}; 

enum CommandType
{
    Command_Clear,
    Command_DrawModel,
};

struct ClearCommand
{
    CommandType type;
    Color color;
};

struct DrawModelCommand
{
    CommandType type;
    Model model;
    u32 group;
};

void command_buffer(CommandBuffer *commands);

RenderGroup *push_render_group();

void push_clear(Color color);
void push_draw_model(Model model, u32 group = 0);


// Asset loading...

struct TextureLoadOp
{
    i32 width;
    i32 height;
    i32 num_channel;
    u8 *data;
};

Model parse_obj(const char *dir, const char *file);

Texture load_texture(char *file);

// Backend...

void opengl_initialize();
void opengl_execute_commands(CommandBuffer *commands, u32 width, u32 height);

Model opengl_load_model(Vertex *vertex_buffer, u32 total_vertices, u32 mesh_count, Mesh *meshes, u32 material_count, Material *material);

Texture opengl_load_texture(TextureLoadOp *op);

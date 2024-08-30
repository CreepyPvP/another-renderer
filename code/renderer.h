#pragma once

#include "game.h"
#include "game_math.h"

#define FRAMEBUFFER_INITIALIZED (1 << 0)
#define FRAMEBUFFER_MULTISAMPLED (1 << 1)
#define FRAMEBUFFER_FILTERED (1 << 2)
#define FRAMEBUFFER_DEPTH (1 << 3)
#define FRAMEBUFFER_DEPTH_TEX (1 << 4)
#define FRAMEBUFFER_COLOR (1 << 5)

enum
{
    Material_DiffuseTexture = (1 << 0),
};

enum 
{
    ShaderLoc_Proj,
    ShaderLoc_Count,
};

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

struct Framebuffer
{
    u32 id;
    u32 flags;

    u32 width;
    u32 height;

    Texture color;
    Texture depth_tex;
    u32 depth;
};

struct Shader
{
    u32 id;
    u32 locs[ShaderLoc_Count];
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
    Command_SetTarget,
    Command_Blit,
    Command_ScreenRect,
};

struct SetRenderTargetCommand
{
    CommandType type;
    Framebuffer *target;
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

struct BlitCommand
{
    CommandType type;

    Framebuffer *dest;
    Framebuffer *source;

    Material material;
};

struct ScreenRectCommand
{
    CommandType type;
    Texture texture;
    Shader *shader;
};

void command_buffer(CommandBuffer *commands);

RenderGroup *push_render_group();

void set_render_target(Framebuffer *target);

void push_blit(Framebuffer *dest, Framebuffer *source);
void push_screen_rect(Texture texture, Shader *shader);
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

Framebuffer opengl_create_framebuffer(u32 width, u32 height, u32 flags);
void opengl_destroy_framebuffer(Framebuffer *framebuffer);

Model opengl_load_model(Vertex *vertex_buffer, u32 total_vertices, u32 mesh_count, Mesh *meshes, u32 material_count, Material *material);
Shader opengl_load_shader(const char* vertex_file, const char* frag_file);
Texture opengl_load_texture(TextureLoadOp *op);

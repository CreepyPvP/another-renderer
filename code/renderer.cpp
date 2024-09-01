#include "renderer.h"
#include "tinyobj_loader_c.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CommandBuffer *active_commands;

void command_buffer(CommandBuffer *commands)
{
    active_commands = commands;

    commands->write = 0;
    commands->render_group_count = 0;
    commands->vertex_count = 0;
    commands->active_draw = NULL;

    push_render_group();
}

#define push_command(type) ((type *) _push_command(sizeof(type)))

void *_push_command(u32 size)
{
    u32 write = active_commands->write;
    assert(write + size <= sizeof(active_commands->memory));
    void *command = &active_commands->memory[write];
    active_commands->write += size;
    active_commands->active_draw = NULL;
    memset(command, 0, size);
    return command;
}

RenderGroup *push_render_group()
{
    assert(active_commands->render_group_count < 10);
    RenderGroup *group = &active_commands->group[active_commands->render_group_count];
    active_commands->render_group_count++;
    return group;
}

void set_render_target(Framebuffer *target)
{
    SetRenderTargetCommand *set_target = push_command(SetRenderTargetCommand);
    set_target->type = Command_SetTarget;
    set_target->target = target;
}

void push_clear(Color color)
{
    ClearCommand *clear = push_command(ClearCommand);
    clear->type = Command_Clear;
    clear->color = color;
}

void push_draw_model(Model model, V3 position, V3 rotation, V3 scale, Material *material, RenderGroup *group)
{
    DrawModelCommand *draw = push_command(DrawModelCommand);

    Mat4 transform = mat_translate(position) * mat_rotate(rotation) * mat_scale(scale);

    draw->type = Command_DrawModel;
    draw->model = model;
    draw->group = group ? group : active_commands->group;
    draw->transform = transform;
    draw->material = material;
}

void push_quad(V3 p0, V3 p1, V3 p2, V3 p3, V3 n, V3 color)
{
    DrawPrimitiveCommand *draw;

    if (active_commands->active_draw) 
    {
        draw = active_commands->active_draw;
    }
    else
    {
        draw = push_command(DrawPrimitiveCommand);
        draw->type = Command_DrawPrimitive;
        draw->group = active_commands->group;
        draw->vertex_offset = active_commands->vertex_count;
        active_commands->active_draw = draw;
    }

    assert(active_commands->vertex_count + 4 < 1024);

    Vertex *vert = &active_commands->vertex_buffer[active_commands->vertex_count];
    vert->position = p0;
    vert->normal = n;
    vert->color = color;
    vert->uv = v2(0, 0);
    vert++;

    vert->position = p1;
    vert->normal = n;
    vert->color = color;
    vert->uv = v2(0, 0);
    vert++;

    vert->position = p2;
    vert->normal = n;
    vert->color = color;
    vert->uv = v2(0, 0);
    vert++;

    vert->position = p3;
    vert->normal = n;
    vert->color = color;
    vert->uv = v2(0, 0);

    active_commands->vertex_count += 4;
    draw->quad_count++;
}

void push_cube(V3 pos, V3 scale, V3 color)
{
    // Up
    push_quad(v3(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(0, 1, 0), color);

    // Down
    push_quad(v3(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(0, -1, 0), color);

    push_quad(v3(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(1, 0, 0), color);

    push_quad(v3(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(-1, 0, 0), color);

    push_quad(v3(pos.x - scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y - scale.y, pos.z + scale.z), 
              v3(pos.x - scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z + scale.z), 
              v3(0, 0, 1), color);

    push_quad(v3(pos.x - scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(pos.x - scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(pos.x + scale.x, pos.y - scale.y, pos.z - scale.z), 
              v3(pos.x + scale.x, pos.y + scale.y, pos.z - scale.z), 
              v3(0, 0, -1), color);
}

void push_blit(Framebuffer *dest, Framebuffer *source)
{
    BlitCommand *blit = push_command(BlitCommand);
    blit->type = Command_Blit;

    assert(source);
    blit->source = source;
    blit->dest = dest;
}

void push_screen_rect(Texture texture, Shader *shader)
{
    ScreenRectCommand *draw = push_command(ScreenRectCommand);
    draw->type = Command_ScreenRect;
    draw->texture = texture;
    draw->shader = shader;
}

Texture load_texture(char *file)
{
    TextureLoadOp load = {};
    load.num_channel = 4;
    
    stbi_set_flip_vertically_on_load(true);
    load.data = stbi_load(file, &load.width, &load.height, &load.num_channel, STBI_default);

    if (!load.data)
    {
        error("Failed to load image: %s", file);
        assert(0);
    }

    Texture texture = opengl_load_texture(&load);

    stbi_image_free(load.data);

    return texture;
}

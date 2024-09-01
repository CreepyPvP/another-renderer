#include "renderer.h"
#include "tinyobj_loader_c.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

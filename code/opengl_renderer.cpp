#include "renderer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct opengl
{
    u32 primitive_vao;
    u32 primitive_buffer;
    u32 quad_vao;

    i32 max_samples;

    u32 render_width;
    u32 render_height;

    Texture white;
    Material default_material;
    Shader default_shader;
};

opengl OPENGL = {};


void APIENTRY debug_output(GLenum source, 
                           GLenum type, 
                           u32 id, 
                           GLenum severity, 
                           GLsizei length, 
                           const char *message, 
                           const void *userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
    {
        return; 
    }
    
    if (severity == GL_DEBUG_SEVERITY_LOW) 
    {
        return;
    }

    const char *source_str;
    const char *type_str;
    const char *severity_str;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             source_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_str = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_str = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_str = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           source_str = "Other"; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_str = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_str = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               type_str = "Other"; break;
    }
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         severity_str = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severity_str = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "notification"; break;
    }

    warn("OPENGL (%s, Source: %s, Type: %s): %s", severity_str, source_str, type_str, message);
}

void opengl_initialize()
{
    assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));

    glGetIntegerv(GL_MAX_SAMPLES, &OPENGL.max_samples);

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

    TextureLoadOp load_white = {};
    load_white.width = 2;
    load_white.height = 2;
    load_white.num_channel = 4;
    
    u8 buffer[16] = {
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
    };

    load_white.data = buffer;

    OPENGL.white = opengl_load_texture(&load_white);
    OPENGL.default_shader = opengl_load_shader("shader/default_vert.glsl", "shader/default_frag.glsl");

    OPENGL.default_material = {};
    OPENGL.default_material.roughness = 1;
    OPENGL.default_material.metallic = 0;
    OPENGL.default_material.base_color = v3(1);

    f32 quad_vertices[5 * 4] = {
        -1, 1, 0, 0, 1,
        1, 1, 0, 1, 1,
        -1, -1, 0, 0, 0,
        1, -1, 0, 1, 0,
    };

    glGenVertexArrays(1, &OPENGL.quad_vao);
    glBindVertexArray(OPENGL.quad_vao);

    u32 quad_vertex_buffer;
    glGenBuffers(1, &quad_vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 5 * 4, quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(f32) * 5, (void*) 0);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(f32) * 5, (void*) (sizeof(f32) * 3));

    glBindVertexArray(0);

    glGenVertexArrays(1, &OPENGL.primitive_vao);
    glGenBuffers(1, &OPENGL.primitive_buffer);
    glBindVertexArray(OPENGL.primitive_vao);
    glBindBuffer(GL_ARRAY_BUFFER, OPENGL.primitive_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, color));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, uv));
    glBindVertexArray(0);
}

void uniform_mat4(u32 id, Mat4 *mat)
{
    glUniformMatrix4fv(id, 1, false, (f32 *) mat);
}

void uniform_v3(u32 id, V3 value)
{
    glUniform3f(id, value.x, value.y, value.z);
}

void uniform_f(u32 id, f32 value)
{
    glUniform1f(id, value);
}

Framebuffer opengl_create_framebuffer(u32 width, u32 height, u32 flags)
{
    Framebuffer res = {};
    res.width = width;
    res.height = height;
    res.flags = flags | FRAMEBUFFER_INITIALIZED;

    glGenFramebuffers(1, &res.id);
    glBindFramebuffer(GL_FRAMEBUFFER, res.id);

    bool multisampled = flags & FRAMEBUFFER_MULTISAMPLED;
    bool filtered = flags & FRAMEBUFFER_FILTERED;
    bool depth = flags & FRAMEBUFFER_DEPTH;
    bool depth_tex = flags & FRAMEBUFFER_DEPTH_TEX;
    bool color = flags & FRAMEBUFFER_COLOR;
    bool hdr = flags & FRAMEBUFFER_HDR;

    assert(!(depth && depth_tex));

    u32 filter = filtered? GL_LINEAR : GL_NEAREST;

    if (color) 
    {
        u32 slot = multisampled? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        glGenTextures(1, &res.color.id);
        glBindTexture(slot, res.color.id);

        u32 format = GL_RGBA;
        if (hdr)
        {
            format = GL_RGBA32F;
        }

        if (multisampled) 
        {
            // GL_RGBA32f for hdr
            glTexImage2DMultisample(slot, OPENGL.max_samples, format, width, height, GL_TRUE);
        } 
        else 
        {
            glTexImage2D(slot, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(slot, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(slot, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(slot, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(slot, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, slot, res.color.id, 0);
        glBindTexture(slot, 0);
    } 
    else 
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (depth) 
    {
        glGenRenderbuffers(1, &res.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, res.depth);

        if (multisampled) 
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, OPENGL.max_samples, GL_DEPTH_COMPONENT, width, height);
        } 
        else 
        {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, res.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    if (depth_tex) 
    {
        glGenTextures(1, &res.depth_tex.id);
        glBindTexture(GL_TEXTURE_2D, res.depth_tex.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, res.depth_tex.id, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    u32 attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    u32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    return res;
}

void opengl_destroy_framebuffer(Framebuffer *framebuffer)
{
    bool initialized = framebuffer->flags & FRAMEBUFFER_INITIALIZED;
    bool depth = framebuffer->flags & FRAMEBUFFER_DEPTH;
    bool color = framebuffer->flags & FRAMEBUFFER_COLOR;
    bool depth_tex = framebuffer->flags & FRAMEBUFFER_DEPTH_TEX;

    if (!initialized) 
    {
        return;
    }

    glDeleteFramebuffers(1, &framebuffer->id);

    if (color) 
    {
        glDeleteTextures(1, &framebuffer->color.id);
    }
    if (depth_tex) 
    {
        glDeleteTextures(1, &framebuffer->depth_tex.id);
    }
    if (depth) 
    {
        glDeleteRenderbuffers(1, &framebuffer->depth);
    }
}

void opengl_clear_buffer(ClearCommand *clear)
{
    Color color = clear->color;
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void opengl_apply_material(Material *material, Texture fallback_diffuse_texture)
{
    Texture diffuse_texture = fallback_diffuse_texture;

    if (material->flags & Material_DiffuseTexture)
    {
        diffuse_texture = material->diffuse;
    }
    glBindTexture(GL_TEXTURE_2D, diffuse_texture.id);

    uniform_v3(OPENGL.default_shader.locs[ShaderLoc_Pbr], v3(material->metallic, material->roughness, 0));
    uniform_v3(OPENGL.default_shader.locs[ShaderLoc_BaseColor], material->base_color);
}

void opengl_draw_model(DrawModelCommand *draw)
{
    glUseProgram(OPENGL.default_shader.id);
    uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Proj], &draw->group->proj);
    uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Model], &draw->transform);
    uniform_v3(OPENGL.default_shader.locs[ShaderLoc_CameraPos], draw->group->camera_pos);

    glBindVertexArray(draw->model.id);
    // glDrawArrays(GL_TRIANGLES, 0, draw->model.total_vertices);
    
    for (u32 i = 0; i < draw->model.mesh_count; ++i)
    {
        Mesh *mesh = &draw->model.meshes[i];
        Material *material = &OPENGL.default_material;

        if (draw->material)
        {
            material = draw->material;
        }
        else
        {
            if (mesh->material_index >= 0)
            {
                material = &draw->model.materials[mesh->material_index];
            }
        }

        opengl_apply_material(material, OPENGL.white);

        glDrawArrays(GL_TRIANGLES, mesh->vertex_offset, mesh->vertex_count);
    }
}

void opengl_draw_primitive(DrawPrimitiveCommand *draw)
{
    glBindVertexArray(OPENGL.primitive_vao);
    glUseProgram(OPENGL.default_shader.id);
    uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Proj], &draw->group->proj);
    uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Model], &mat4(1));
    uniform_v3(OPENGL.default_shader.locs[ShaderLoc_CameraPos], draw->group->camera_pos);

    opengl_apply_material(&OPENGL.default_material, OPENGL.white);

    i32 first_buffer[128];
    i32 count_buffer[128];

    u32 face_count = 0;
    u32 vertex_offset = draw->vertex_offset;
    while (face_count < draw->quad_count)
    {
        u32 batch_face;
        for (batch_face = 0; batch_face < 128 && face_count < draw->quad_count; ++batch_face)
        {
            first_buffer[batch_face] = vertex_offset + face_count * 4;
            count_buffer[batch_face] = 4;

            face_count++;
        }

        glMultiDrawArrays(GL_TRIANGLE_STRIP, first_buffer, count_buffer, batch_face);
    }
}

void opengl_set_render_target(Framebuffer *target)
{
    u32 width = target? target->width : OPENGL.render_width;
    u32 height = target? target->height : OPENGL.render_height;
    u32 id = target? target->id : 0;

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void opengl_blit(BlitCommand *blit)
{
    // opengl_set_render_target(blit->dest);
    // 
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);
    //
    // glUseProgram(OPENGL.default_shader.id);
    // uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Proj], &mat4(1));
    //
    // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, blit->source.id);
    //
    // glBindVertexArray(OPENGL.quad_vao);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    u32 dest_id = blit->dest? blit->dest->id : 0;
    u32 dest_width = blit->dest? blit->dest->width : OPENGL.render_width;
    u32 dest_height = blit->dest? blit->dest->height : OPENGL.render_height;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, blit->source->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest_id);

    assert(blit->source->width == dest_width);
    glBlitFramebuffer(0, 0, blit->source->width, blit->source->height, 0, 0, dest_width, dest_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void opengl_screen_rect(ScreenRectCommand *screen_rect)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);

    glUseProgram(screen_rect->shader->id);
    glBindTexture(GL_TEXTURE_2D, screen_rect->texture.id);

    glBindVertexArray(OPENGL.quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
}

void opengl_execute_commands(CommandBuffer *commands, u32 width, u32 height)
{
    OPENGL.render_width = width;
    OPENGL.render_height = height;

    opengl_set_render_target(NULL);

    glBindBuffer(GL_ARRAY_BUFFER, OPENGL.primitive_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * commands->vertex_count, commands->vertex_buffer, GL_STREAM_DRAW);

    u32 offset = 0;
    while (offset < commands->write)
    {
        CommandType *type = (CommandType *) advance_pointer(commands->memory, offset);

        switch (*type)
        {
            case Command_Clear: {
                ClearCommand *clear = (ClearCommand *) advance_pointer(commands->memory, offset);
                opengl_clear_buffer(clear);
                offset += sizeof(ClearCommand);
                break;
            }

            case Command_DrawModel: {
                DrawModelCommand *draw = (DrawModelCommand *) advance_pointer(commands->memory, offset);
                opengl_draw_model(draw);
                offset += sizeof(DrawModelCommand);
                break;
            }

            case Command_SetTarget: {
                SetRenderTargetCommand *set_target = (SetRenderTargetCommand *) advance_pointer(commands->memory, offset);
                opengl_set_render_target(set_target->target);
                offset += sizeof(SetRenderTargetCommand);
                break;
            }

            case Command_Blit: {
                BlitCommand *blit = (BlitCommand *) advance_pointer(commands->memory, offset);
                opengl_blit(blit);
                offset += sizeof(BlitCommand);
                break;
            }

            case Command_ScreenRect: {
                ScreenRectCommand *screen_rect = (ScreenRectCommand *) advance_pointer(commands->memory, offset);
                opengl_screen_rect(screen_rect);
                offset += sizeof(ScreenRectCommand);
                break;
            }

            case Command_DrawPrimitive: {
                DrawPrimitiveCommand *draw = (DrawPrimitiveCommand *) advance_pointer(commands->memory, offset);
                opengl_draw_primitive(draw);
                offset += sizeof(DrawPrimitiveCommand);
                break;
            }

            default: {
                assert(0);
            }
        }

        assert(glGetError() == GL_NO_ERROR);
    }
}

Shader opengl_load_shader(const char* vertex_file, const char* frag_file)
{
    char info_log[512];
    i32 status;

    u8 *vertex_code = read_file(vertex_file, NULL);
    assert(vertex_code);
    u32 vertex_prog = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_prog, 1, (char **)  &vertex_code, NULL);
    glCompileShader(vertex_prog);
    glGetShaderiv(vertex_prog, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertex_prog, 512, NULL, info_log);
        error("Error compiling vertex shader (%s): %s", vertex_file, info_log);
        assert(0);
    }

    u8 *fragment_code = read_file(frag_file, NULL);
    assert(fragment_code);
    u32 frag_prog = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_prog, 1, (char **) &fragment_code, NULL);
    glCompileShader(frag_prog);
    glGetShaderiv(frag_prog, GL_COMPILE_STATUS, &status);
    if(!status) {
        glGetShaderInfoLog(frag_prog, 512, NULL, info_log);
        error("Error compiling fragment shader (%s): %s", frag_file, info_log);
        assert(0);
    }

    Shader shader;
    shader.id = glCreateProgram();
    glAttachShader(shader.id, vertex_prog);
    glAttachShader(shader.id, frag_prog);
    glLinkProgram(shader.id);
    glGetProgramiv(shader.id, GL_LINK_STATUS, &status);

    if(!status) {
        glGetProgramInfoLog(shader.id, 512, NULL, info_log);
        error("Error linking shader: %s", info_log);
        assert(0);
    }

    glDeleteShader(vertex_prog);
    glDeleteShader(frag_prog);

    shader.locs[ShaderLoc_Proj] = glGetUniformLocation(shader.id, "proj");
    shader.locs[ShaderLoc_Model] = glGetUniformLocation(shader.id, "model");
    shader.locs[ShaderLoc_CameraPos] = glGetUniformLocation(shader.id, "camera_pos");
    shader.locs[ShaderLoc_Pbr] = glGetUniformLocation(shader.id, "pbr");
    shader.locs[ShaderLoc_BaseColor] = glGetUniformLocation(shader.id, "base_color");

    return shader;
}

Model opengl_load_model(Vertex *vertex_buffer, u32 total_vertices, u32 mesh_count, Mesh *meshes, u32 material_count, Material *materials)
{
    Model model = {};

    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vert_buffer;
    glGenBuffers(1, &vert_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * total_vertices, vertex_buffer, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, color));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, uv));

    glBindVertexArray(0);

    model.id = vao;
    model.total_vertices = total_vertices;
    model.mesh_count = mesh_count;
    model.meshes = (Mesh *) malloc(sizeof(Mesh) * mesh_count);
    model.material_count = material_count;
    model.materials = (Material *) malloc(sizeof(Material) * material_count);

    memcpy(model.meshes, meshes, sizeof(Mesh) * mesh_count);
    memcpy(model.materials, materials, sizeof(Material) * material_count);

    return model;
}

Texture opengl_load_texture(TextureLoadOp *load)
{
    Texture texture = {};
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    u32 format = GL_RGBA;
    if (load->num_channel == 3) 
    {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, load->width, load->height, 0, format, GL_UNSIGNED_BYTE, load->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

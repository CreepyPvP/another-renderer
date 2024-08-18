#include "renderer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum ShaderLoc
{
    ShaderLoc_Proj,
    ShaderLoc_Count,
};

struct Shader
{
    u32 id;
    u32 locs[ShaderLoc_Count];
};

struct opengl
{
    i32 max_samples;

    Shader default_shader;
};

opengl OPENGL;

Shader load_shader(const char* vertex_file, const char* frag_file);


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

void initialize_backend()
{
    assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));

    glGetIntegerv(GL_MAX_SAMPLES, &OPENGL.max_samples);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

    OPENGL.default_shader = load_shader("shader/default_vert.glsl", "shader/default_frag.glsl");
}

void uniform_mat4(u32 id, Mat4 *mat)
{
    glUniformMatrix4fv(id, 1, false, (f32 *) mat);
}

void clear_buffer(ClearCommand *clear)
{
    Color color = clear->color;
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void draw_model(DrawModelCommand *draw, CommandBuffer *commands)
{
    glUseProgram(OPENGL.default_shader.id);
    uniform_mat4(OPENGL.default_shader.locs[ShaderLoc_Proj], &commands->group[draw->group].proj);

    glBindVertexArray(draw->model.id);
    // glDrawArrays(GL_TRIANGLES, 0, draw->model.total_vertices);
    
    for (u32 i = 0; i < draw->model.mesh_count; ++i)
    {
        Mesh *mesh = &draw->model.meshes[i];
        // TODO: Set material uniforms here
        glDrawArrays(GL_TRIANGLES, mesh->vertex_offset, mesh->vertex_count);
    }
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
                clear_buffer(clear);
                offset += sizeof(ClearCommand);
                break;
            }

            case Command_DrawModel: {
                DrawModelCommand *draw = (DrawModelCommand *) advance_pointer(commands->memory, offset);
                draw_model(draw, commands);
                offset += sizeof(DrawModelCommand);
                break;
            }

            default: {
                assert(0);
            }
        }
    }
}

Shader load_shader(const char* vertex_file, const char* frag_file)
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

    return shader;
}

Model load_model(Vertex *vertex_buffer, u32 total_vertices, u32 mesh_count, Mesh *meshes, u32 material_count, Material *materials)
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

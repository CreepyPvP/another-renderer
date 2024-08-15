#include "game.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

void filereader_callback(void *ctx, const char *filename, i32 is_mtl, const char *obj_filename, char **buffer, size_t *length)
{
    u32 buffer_size = KiloBytes(128);
    *buffer = (char *) malloc(buffer_size);
    *length = read_file(filename, *buffer, buffer_size);
}

void parse_obj(const char *path)
{
    tinyobj_attrib_t attrib = {};

    tinyobj_shape_t *meshes = NULL;
    u32 num_meshes = 0;

    tinyobj_material_t *materials = NULL;
    u32 material_count = 0;

    u32 flags = 0;

    i32 result = tinyobj_parse_obj(&attrib, &meshes, &num_meshes, &materials, &material_count, path, filereader_callback, NULL, flags);

    if (result != TINYOBJ_SUCCESS)
    {
        error("Failed to parse OBJ %s", path);
        assert(0);
    }
}

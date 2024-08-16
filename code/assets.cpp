#include "game.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

void filereader_callback(void *ctx, const char *filename, i32 is_mtl, const char *obj_filename, char **buffer, size_t *length)
{
    *length = 0;
    *buffer = (char *) read_file(filename, (u32 *) length);
}

void parse_obj(const char *path)
{
    tinyobj_attrib_t attrib = {};

    tinyobj_shape_t *meshes = NULL;
    size_t mesh_count = 0;

    tinyobj_material_t *materials = NULL;
    size_t material_count = 0;

    u32 flags = 0;

    i32 result = tinyobj_parse_obj(&attrib, &meshes, &mesh_count, &materials, &material_count, path, filereader_callback, NULL, flags);

    assert(result == TINYOBJ_SUCCESS);
    assert(mesh_count);
}

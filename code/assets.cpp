#include "game.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

void parse_obj(const char *path)
{
    if (tinyobj_parse_obj(tinyobj_attrib_t *attrib, tinyobj_shape_t **shapes,
                             size_t *num_shapes, tinyobj_material_t **materials,
                             size_t *num_materials, const char *file_name, file_reader_callback file_reader,
                             void *ctx, unsigned int flags) != TINYOBJ_SUCCESS)
    {
        error("Failed to parse OBJ %s", path);
        assert(0);
    }
}

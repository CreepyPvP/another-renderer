#include "game.h"

#include <stdlib.h>
#include <stdio.h>

#include "renderer.h"
#include "game_math.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
namespace obj
{
#include "tinyobj_loader_c.h"
}

void filereader_callback(void *ctx, const char *filename, i32 is_mtl, const char *obj_filename, char **buffer, size_t *length)
{
    *length = 0;
    *buffer = (char *) read_file(filename, (u32 *) length);
}

Model parse_obj(const char *dir, const char *file)
{
    char path[256];
    sprintf(path, "%s/%s", dir, file);

    obj::tinyobj_attrib_t attrib = {};

    obj::tinyobj_shape_t *meshes = NULL;
    size_t mesh_count = 0;

    obj::tinyobj_material_t *materials = NULL;
    size_t material_count = 0;

    u32 flags = TINYOBJ_FLAG_TRIANGULATE;
    i32 result = obj::tinyobj_parse_obj(&attrib, &meshes, &mesh_count, &materials, &material_count, path, filereader_callback, NULL, flags);

    assert(result == TINYOBJ_SUCCESS);
    assert(mesh_count);

    u32 triangle_count = attrib.num_face_num_verts;
    Vertex *vertex_buffer = (Vertex *) malloc(triangle_count * 3 * sizeof(Vertex));

    u32 vertex_index = 0;

    for (u32 face = 0; face < triangle_count; ++face)
    {
        for (u32 i = 0; i < 3; ++i)
        {
            Vertex vertex = {};

            i32 v_index = attrib.faces[face * 3 + i].v_idx;
            i32 vn_index = attrib.faces[face * 3 + i].vn_idx;
            i32 vt_index = attrib.faces[face * 3 + i].vt_idx;
            i32 material_id = attrib.material_ids[face];

            vertex.position.x = attrib.vertices[v_index * 3 + 0];
            vertex.position.y = attrib.vertices[v_index * 3 + 1];
            vertex.position.z = attrib.vertices[v_index * 3 + 2];

            vertex.normal.x = attrib.normals[vn_index * 3 + 0];
            vertex.normal.y = attrib.normals[vn_index * 3 + 1];
            vertex.normal.z = attrib.normals[vn_index * 3 + 2];

            vertex.uv.x = attrib.texcoords[vt_index * 2 + 0];
            vertex.uv.y = attrib.texcoords[vt_index * 2 + 1];

            if (material_id != -1)
            {
                obj::tinyobj_material_t *material = &materials[material_id];
                vertex.color = v3(material->diffuse[0], material->diffuse[1], material->diffuse[2]);
            }
            else
            {
                vertex.color = v3(1, 1, 1);
            }

            vertex_buffer[face * 3 + i] = vertex;
        }
    }

    Mesh *model_meshes = (Mesh *) malloc(sizeof(Mesh) * mesh_count);
    Material *model_materials = (Material *) malloc(sizeof(Material) * material_count);

    for (u32 i = 0; i < mesh_count; ++i)
    {
        Mesh *mesh = &model_meshes[i];
        obj::tinyobj_shape_t *shape = &meshes[i];

        mesh->vertex_count = shape->length * 3;
        mesh->vertex_offset = shape->face_offset * 3;
        mesh->material_index = attrib.material_ids[shape->face_offset];
    }

    for (u32 i = 0; i < material_count; ++i)
    {
        Material *material = &model_materials[i];
        *material = {};

        // 'materials[i].diffuse_texname' contains values like "textures/some_texture.png"
        char *diffuse_texture = materials[i].diffuse_texname;
        if (diffuse_texture)
        {
            sprintf(path, "%s/%s", dir, diffuse_texture);
            material->diffuse = load_texture(path);
            material->flags |= Material_DiffuseTexture;
        }
    }

    return opengl_load_model(vertex_buffer, triangle_count * 3, mesh_count, model_meshes, material_count, model_materials);
}

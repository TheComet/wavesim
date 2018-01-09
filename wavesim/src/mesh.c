#include "wavesim/mesh.h"
#include "wavesim/memory.h"
#include <string.h>
#include <math.h>

static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);
static void init_attribute_buffer(mesh_t* mesh, int vertex_count);
static void calculate_aabb(mesh_t* mesh);

/* ------------------------------------------------------------------------- */
mesh_builder_t*
mesh_builder_create(void)
{
    mesh_builder_t* mb = MALLOC(sizeof(*mb));
    if (mb == NULL)
        OUT_OF_MEMORY(NULL);
    mb->aabb = aabb_reset();
    return mb;
}

/* ------------------------------------------------------------------------- */
void
mesh_builder_destroy(mesh_builder_t* mb)
{
    (void)mb;
}

/* ------------------------------------------------------------------------- */
int
mesh_builder_add_face(mesh_builder_t* mb, face_t face)
{
    face_t* face_store = vector_emplace(&mb->faces);
    if (face_store == NULL)
        OUT_OF_MEMORY(-1);

    *face_store = face;

    /* Update bounding box, maybe the new face has made it grow */
#define UPDATE(vertex, axis) \
        if ((vertex).position.v.axis < mb->aabb.a.v.axis) \
            mb->aabb.a.v.axis = (vertex).position.v.axis; \
        if ((vertex).position.v.axis > mb->aabb.b.v.axis) \
            mb->aabb.b.v.axis = (vertex).position.v.axis;
    UPDATE(face.vertices[0], x); UPDATE(face.vertices[0], y); UPDATE(face.vertices[0], z);
    UPDATE(face.vertices[1], x); UPDATE(face.vertices[1], y); UPDATE(face.vertices[1], z);
    UPDATE(face.vertices[2], x); UPDATE(face.vertices[2], y); UPDATE(face.vertices[2], z);
#undef UPDATE

    return 0;
}

/* ------------------------------------------------------------------------- */
mesh_t*
mesh_builder_finalize(mesh_builder_t* mb)
{
    (void)mb;
    return NULL;
}

/* ------------------------------------------------------------------------- */
mesh_t*
mesh_create(void)
{
    mesh_t* mesh = (mesh_t*)MALLOC(sizeof *mesh);
    if (mesh == NULL)
        OUT_OF_MEMORY(NULL);
    mesh_construct(mesh);
    return mesh;
}

/* ------------------------------------------------------------------------- */
void
mesh_construct(mesh_t* mesh)
{
    mesh->ab = NULL;
    mesh->vb = NULL;
    mesh->ib = NULL;
}

/* ------------------------------------------------------------------------- */
void
mesh_destruct(mesh_t* mesh)
{
    mesh_clear_buffers(mesh);
}

/* ------------------------------------------------------------------------- */
void
mesh_destroy(mesh_t* mesh)
{
    mesh_destruct(mesh);
    FREE(mesh);
}

/* ------------------------------------------------------------------------- */
void
mesh_clear_buffers(mesh_t* mesh)
{
    if (mesh->ab != NULL)
    {
        FREE(mesh->ab);

        if (mesh->we_own_the_buffers)
        {
            FREE(mesh->vb);
            FREE(mesh->ib);
        }
    }

    mesh->ab = NULL;
    mesh->vb = NULL;
    mesh->ib = NULL;
}

/* ------------------------------------------------------------------------- */
int
mesh_assign_buffers(mesh_t* mesh,
                    void* vertex_buffer, void* index_buffer,
                    int vertex_count, int index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(vertex_attrs_t) * vertex_count)) == NULL)
        OUT_OF_MEMORY(-1);
    mesh->vb = vertex_buffer;
    mesh->ib = index_buffer;
    mesh->vb_count = vertex_count;
    mesh->ib_count = index_count;
    mesh->we_own_the_buffers = 0;

    init_attribute_buffer(mesh, vertex_count);
    calculate_aabb(mesh);

    return 0;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API int
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       int vertex_count, int index_count,
                       mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(vertex_attrs_t) * vertex_count)) == NULL)
        goto ab_alloc_failed;
    if ((mesh->vb = MALLOC(mesh->vertex_size * vertex_count)) == NULL)
        goto vb_alloc_failed;
    if ((mesh->ib = MALLOC(mesh->index_size * index_count)) == NULL)
        goto ib_alloc_failed;

    memcpy(mesh->vb, vertex_buffer, mesh->vertex_size * vertex_count);
    memcpy(mesh->ib, index_buffer, mesh->index_size * index_count);

    mesh->vb_count = vertex_count;
    mesh->ib_count = index_count;
    mesh->we_own_the_buffers = 1;

    init_attribute_buffer(mesh, vertex_count);
    calculate_aabb(mesh);

    return 0;

    ib_alloc_failed: FREE(mesh->vb);
    vb_alloc_failed: FREE(mesh->ab);
    ab_alloc_failed: return -1;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API vec3_t
mesh_get_vertex_position(mesh_t* mesh, int index)
{
    index *= 3;

#define CONSTRUCT_VEC3(T) \
    vec3( \
        (real)*(T*)((uint8_t*)mesh->vb + index + 0), \
        (real)*(T*)((uint8_t*)mesh->vb + index + 1), \
        (real)*(T*)((uint8_t*)mesh->vb + index + 2) \
    );

    switch (mesh->vb_type)
    {
        case MESH_VB_FLOAT       : return CONSTRUCT_VEC3(float);
        case MESH_VB_DOUBLE      : return CONSTRUCT_VEC3(double);
        case MESH_VB_LONG_DOUBLE : return CONSTRUCT_VEC3(long double);
    }
#undef CONSTRUCT_VEC3
    return vec3(0, 0, 0);
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    unsigned int index_size = (ib_type / 2);
    index_size = (8 << index_size);
    mesh->index_size = index_size;

    mesh->vb_type = vb_type;
    mesh->ib_type = ib_type;

    switch (vb_type)
    {
        case MESH_VB_FLOAT       : mesh->vertex_size = sizeof(float); break;
        case MESH_VB_DOUBLE      : mesh->vertex_size = sizeof(double); break;
        case MESH_VB_LONG_DOUBLE : mesh->vertex_size = sizeof(long double); break;
    }
}

/* ------------------------------------------------------------------------- */
static void init_attribute_buffer(mesh_t* mesh, int vertex_count)
{
    int i;
    for (i = 0; i != vertex_count; ++i)
        mesh->ab[i] = (vertex_attrs_t){0};
}

/* ------------------------------------------------------------------------- */
static void calculate_aabb(mesh_t* mesh)
{
    int v, i;
    mesh->aabb = aabb(-INFINITY, -INFINITY, -INFINITY, INFINITY, INFINITY, INFINITY);

    for (v = 0; v != mesh->vb_count; ++v)
    {
        vec3_t pos = mesh_get_vertex_position(mesh, v);
        for (i = 0; i != 3; ++i)
        {
            if (pos.f[i] < mesh->aabb.a.f[i])
                mesh->aabb.a.f[i] = pos.f[i];
            if (pos.f[i] > mesh->aabb.b.f[i])
                mesh->aabb.b.f[i] = pos.f[i];
        }
    }
}

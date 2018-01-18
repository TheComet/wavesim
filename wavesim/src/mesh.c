#include "wavesim/mesh.h"
#include "wavesim/memory.h"
#include <string.h>
#include <math.h>

static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);
static void init_attribute_buffer(mesh_t* mesh, int vertex_count);
static void calculate_aabb(mesh_t* mesh);

/* ------------------------------------------------------------------------- */
mesh_t*
mesh_create(void)
{
    mesh_t* mesh = MALLOC(sizeof *mesh);
    if (mesh == NULL)
        OUT_OF_MEMORY(NULL);
    mesh_construct(mesh);
    return mesh;
}

/* ------------------------------------------------------------------------- */
void
mesh_construct(mesh_t* mesh)
{
    memset(mesh, 0, sizeof *mesh);
    mesh->aabb = aabb_reset();
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
                    WS_IB vertex_count, WS_IB index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(attribute_t) * vertex_count)) == NULL)
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
                       WS_IB vertex_count, WS_IB index_count,
                       mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(attribute_t) * vertex_count)) == NULL)
        goto ab_alloc_failed;
    if ((mesh->vb = MALLOC(mesh->vb_size * vertex_count * 3)) == NULL)
        goto vb_alloc_failed;
    if ((mesh->ib = MALLOC(mesh->ib_size * index_count)) == NULL)
        goto ib_alloc_failed;

    memcpy(mesh->vb, vertex_buffer, mesh->vb_size * vertex_count * 3);
    memcpy(mesh->ib, index_buffer, mesh->ib_size * index_count);

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
vec3_t
mesh_get_vertex_position(mesh_t* mesh, WS_IB index)
{
    return mesh_get_vertex_position_from_buffer(mesh->vb, index, mesh->vb_type);
}

/* ------------------------------------------------------------------------- */
vec3_t
mesh_get_vertex_position_from_buffer(void* vb, WS_IB index, mesh_vb_type_e vb_type)
{
    index *= 3;

#define CONSTRUCT_VEC3(T) \
    vec3( \
        (WS_REAL)*((T*)vb + index + 0), \
        (WS_REAL)*((T*)vb + index + 1), \
        (WS_REAL)*((T*)vb + index + 2) \
    );

    switch (vb_type)
    {
        case MESH_VB_FLOAT       : return CONSTRUCT_VEC3(float);
        case MESH_VB_DOUBLE      : return CONSTRUCT_VEC3(double);
        case MESH_VB_LONG_DOUBLE : return CONSTRUCT_VEC3(long double);
    }
#undef CONSTRUCT_VEC3
    return vec3(0, 0, 0);
}

/* ------------------------------------------------------------------------- */
WS_IB
mesh_get_index_from_buffer(void* ib, WS_IB index, mesh_ib_type_e ib_type)
{
    switch (ib_type)
    {
        case MESH_IB_INT8   : return (WS_IB)*((int8_t*)ib + index);
        case MESH_IB_UINT8  : return (WS_IB)*((uint8_t*)ib + index);
        case MESH_IB_INT16  : return (WS_IB)*((int16_t*)ib + index);
        case MESH_IB_UINT16 : return (WS_IB)*((uint16_t*)ib + index);
        case MESH_IB_INT32  : return (WS_IB)*((int32_t*)ib + index);
        case MESH_IB_UINT32 : return (WS_IB)*((uint32_t*)ib + index);
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
        case MESH_IB_INT64  : return (WS_IB)*((int64_t*)ib + index);
        case MESH_IB_UINT64 : return (WS_IB)*((uint64_t*)ib + index);
#endif
    }
    return (WS_IB)-1;
}

/* ------------------------------------------------------------------------- */
face_t
mesh_get_face_from_buffers(void* vb, void* ib, attribute_t* attrs,
                           WS_IB face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
	WS_IB indices[3];
	vec3_t vertices[3];

	indices[0] = mesh_get_index_from_buffer(ib, face_index * 3 + 0, ib_type);
	indices[1] = mesh_get_index_from_buffer(ib, face_index * 3 + 1, ib_type);
	indices[2] = mesh_get_index_from_buffer(ib, face_index * 3 + 2, ib_type);

	vertices[0] = mesh_get_vertex_position_from_buffer(vb, indices[0], vb_type);
	vertices[1] = mesh_get_vertex_position_from_buffer(vb, indices[1], vb_type);
	vertices[2] = mesh_get_vertex_position_from_buffer(vb, indices[2], vb_type);

    return face(
        vertex(vertices[0], attrs[indices[0]]),
        vertex(vertices[1], attrs[indices[1]]),
        vertex(vertices[2], attrs[indices[2]])
    );
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    unsigned int index_size = (ib_type / 2);
    index_size = (1 << index_size);
    mesh->ib_size = index_size;

    mesh->vb_type = vb_type;
    mesh->ib_type = ib_type;

    switch (vb_type)
    {
        case MESH_VB_FLOAT       : mesh->vb_size = sizeof(float); break;
        case MESH_VB_DOUBLE      : mesh->vb_size = sizeof(double); break;
        case MESH_VB_LONG_DOUBLE : mesh->vb_size = sizeof(long double); break;
    }
}

/* ------------------------------------------------------------------------- */
static void init_attribute_buffer(mesh_t* mesh, int vertex_count)
{
    int i;
    for (i = 0; i != vertex_count; ++i)
        mesh->ab[i] = (attribute_t){0, 0, 1}; /* full absorption by default */
}

/* ------------------------------------------------------------------------- */
static void calculate_aabb(mesh_t* mesh)
{
    WS_IB v, i;
    mesh->aabb = aabb_reset();

    for (v = 0; v != mesh->vb_count; ++v)
    {
        vec3_t pos = mesh_get_vertex_position(mesh, v);
        for (i = 0; i != 3; ++i)
        {
            if (pos.xyz[i] < mesh->aabb.b.min.xyz[i])
                mesh->aabb.b.min.xyz[i] = pos.xyz[i];
            if (pos.xyz[i] > mesh->aabb.b.max.xyz[i])
                mesh->aabb.b.max.xyz[i] = pos.xyz[i];
        }
    }
}

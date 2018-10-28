#include "wavesim/memory.h"
#include "wavesim/btree.h"
#include "wavesim/hash.h"
#include "wavesim/mesh/mesh.h"
#include <string.h>
#include <math.h>

static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);
static void init_attribute_buffer(mesh_t* mesh, size_t vertex_count);
static void calculate_aabb(mesh_t* mesh);

/* ------------------------------------------------------------------------- */
wsret
mesh_create(mesh_t** mesh, const char* name)
{
    wsret result;

    *mesh = MALLOC(sizeof **mesh);
    if (*mesh == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    if ((result = mesh_construct(*mesh, name)) != WS_OK)
        return result;

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
wsret
mesh_construct(mesh_t* mesh, const char* name)
{
    size_t name_len = strlen(name);

    memset(mesh, 0, sizeof *mesh);
    mesh->aabb = aabb_reset();

    mesh->name = MALLOC(sizeof(char) * (name_len + 1));
    if (mesh->name == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    strcpy(mesh->name, name);

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
mesh_destruct(mesh_t* mesh)
{
    mesh_clear_buffers(mesh);
    FREE(mesh->name);
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
wsret
mesh_assign_buffers(mesh_t* mesh,
                    void* vertex_buffer, void* index_buffer,
                    size_t vertex_count, size_t index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(attribute_t) * vertex_count)) == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    mesh->vb = vertex_buffer;
    mesh->ib = index_buffer;
    mesh->vb_count = vertex_count;
    mesh->ib_count = index_count;
    mesh->we_own_the_buffers = 0;

    init_attribute_buffer(mesh, vertex_count);
    calculate_aabb(mesh);

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
wsret
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       size_t vertex_count, size_t index_count,
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

    WSRET(WS_OK);

    ib_alloc_failed: FREE(mesh->vb);
    vb_alloc_failed: FREE(mesh->ab);
    ab_alloc_failed: WSRET(WS_ERR_OUT_OF_MEMORY);
}

/* ------------------------------------------------------------------------- */
vec3_t
mesh_get_vertex_position(const mesh_t* mesh, wsib_t index)
{
    return mesh_get_vertex_position_from_buffer(mesh->vb, index, mesh->vb_type);
}

/* ------------------------------------------------------------------------- */
vec3_t
mesh_get_vertex_position_from_buffer(const void* vb, wsib_t index, mesh_vb_type_e vb_type)
{
    index *= 3;

#define CONSTRUCT_VEC3(T) \
    vec3( \
        (wsreal_t)*((T*)vb + index + 0), \
        (wsreal_t)*((T*)vb + index + 1), \
        (wsreal_t)*((T*)vb + index + 2) \
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
wsib_t
mesh_get_index_from_buffer(const void* ib, size_t index, mesh_ib_type_e ib_type)
{
    switch (ib_type)
    {
        case MESH_IB_INT8   : return (wsib_t)*((int8_t*)ib + index);
        case MESH_IB_UINT8  : return (wsib_t)*((uint8_t*)ib + index);
        case MESH_IB_INT16  : return (wsib_t)*((int16_t*)ib + index);
        case MESH_IB_UINT16 : return (wsib_t)*((uint16_t*)ib + index);
        case MESH_IB_INT32  : return (wsib_t)*((int32_t*)ib + index);
        case MESH_IB_UINT32 : return (wsib_t)*((uint32_t*)ib + index);
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
        case MESH_IB_INT64  : return (wsib_t)*((int64_t*)ib + index);
        case MESH_IB_UINT64 : return (wsib_t)*((uint64_t*)ib + index);
#endif
    }
    return (wsib_t)-1;
}

/* ------------------------------------------------------------------------- */
face_t
mesh_get_face(const mesh_t* mesh, size_t face_index)
{
    return mesh_get_face_from_buffers(mesh->vb, mesh->ib, mesh->ab, face_index, mesh->vb_type, mesh->ib_type);
}

/* ------------------------------------------------------------------------- */
face_t
mesh_get_face_from_buffers(const void* vb, const void* ib, const attribute_t* attrs,
                           size_t face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    wsib_t indices[3];
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
void
mesh_write_index_to_buffer(void* ib, size_t offset, wsib_t index, mesh_ib_type_e ib_type)
{
    switch (ib_type)
    {
        case MESH_IB_INT8   : *((int8_t*)ib + offset)   = (int8_t)index;   break;
        case MESH_IB_UINT8  : *((uint8_t*)ib + offset)  = (uint8_t)index;  break;
        case MESH_IB_INT16  : *((int16_t*)ib + offset)  = (int16_t)index;  break;
        case MESH_IB_UINT16 : *((uint16_t*)ib + offset) = (uint16_t)index; break;
        case MESH_IB_INT32  : *((int32_t*)ib + offset)  = (int32_t)index;  break;
        case MESH_IB_UINT32 : *((uint32_t*)ib + offset) = (uint32_t)index; break;
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
        case MESH_IB_INT64  : *((int64_t*)ib + offset)  = (int64_t)index;  break;
        case MESH_IB_UINT64 : *((uint64_t*)ib + offset) = (uint64_t)index; break;
#endif
    }
}


/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    unsigned int index_size = (ib_type / 2);
    index_size = (1u << index_size);
    mesh->ib_size = (uint8_t)index_size;

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
static void init_attribute_buffer(mesh_t* mesh, size_t vertex_count)
{
    size_t i;
    for (i = 0; i != vertex_count; ++i)
        attribute_set_default_solid(&mesh->ab[i]);
}

/* ------------------------------------------------------------------------- */
static void calculate_aabb(mesh_t* mesh)
{
    size_t v, i;
    mesh->aabb = aabb_reset();

    for (v = 0; v != mesh->vb_count; ++v)
    {
        vec3_t pos = mesh_get_vertex_position(mesh, (wsib_t)v);
        for (i = 0; i != 3; ++i)
        {
            if (pos.xyz[i] < mesh->aabb.b.min.xyz[i])
                mesh->aabb.b.min.xyz[i] = pos.xyz[i];
            if (pos.xyz[i] > mesh->aabb.b.max.xyz[i])
                mesh->aabb.b.max.xyz[i] = pos.xyz[i];
        }
    }
}

/* ------------------------------------------------------------------------- */
int
mesh_is_manifold(const mesh_t* mesh)
{
    /* Condition is V + F - E = 2 */
    size_t V, F, E, idx;
    btree_t set;

    /* Have to count the number of unique edges in the mesh. */
    V = mesh_vertex_count(mesh);
    F = mesh_face_count(mesh);
    btree_construct(&set);
    /* Iterate 3 indices at a time (i.e. iterate faces)*/
    for (idx = 0; idx != mesh_index_count(mesh); idx += 3)
    {
        wsib_t e1[2];
        wsib_t e2[2];
        wsib_t e3[2];
        wsib_t indices[3];

        indices[0] = mesh_get_index_from_buffer(mesh->ib, idx + 0, mesh->ib_type);
        indices[1] = mesh_get_index_from_buffer(mesh->ib, idx + 1, mesh->ib_type);
        indices[2] = mesh_get_index_from_buffer(mesh->ib, idx + 2, mesh->ib_type);

        /* Need to sort the indices so "flipped" edges resolve to the same hash value */
        e1[0] = indices[0] < indices[1] ? indices[0] : indices[1];
        e1[1] = indices[0] > indices[1] ? indices[0] : indices[1];
        e2[0] = indices[1] < indices[2] ? indices[1] : indices[2];
        e2[1] = indices[1] > indices[2] ? indices[1] : indices[2];
        e3[0] = indices[2] < indices[0] ? indices[2] : indices[0];
        e3[1] = indices[2] > indices[0] ? indices[2] : indices[0];

        if (btree_insert(&set, hash32_edge_indices(e1), (void*)(size_t)indices[0]) < -1)
            goto ran_out_of_memory;
        if (btree_insert(&set, hash32_edge_indices(e2), (void*)(size_t)indices[1]) < -1)
            goto ran_out_of_memory;
        if (btree_insert(&set, hash32_edge_indices(e3), (void*)(size_t)indices[2]) < -1)
            goto ran_out_of_memory;
    }

    E = btree_count(&set);
    btree_clear_free(&set);

    if (V + F - E == 2)
        return 1;
    return 0;

    ran_out_of_memory : btree_clear_free(&set);
    return -1;
}

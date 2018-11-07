#include "wavesim/memory.h"
#include "wavesim/hashmap.h"
#include "wavesim/mesh/mesh.h"
#include <string.h>
#include <math.h>

static void set_vb_ib_types_and_sizes(mesh_t* mesh, mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);
static void init_attribute_buffer(mesh_t* mesh, uintptr_t vertex_count);
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

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
wsret
mesh_construct(mesh_t* mesh, const char* name)
{
    uintptr_t name_len = strlen(name);

    memset(mesh, 0, sizeof *mesh);
    mesh->aabb = aabb_reset();

    mesh->name = MALLOC(sizeof(char) * (name_len + 1));
    if (mesh->name == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    strcpy(mesh->name, name);

    WSRET(WS_OK);
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
                    uintptr_t vertex_count, uintptr_t index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    mesh_clear_buffers(mesh);
    set_vb_ib_types_and_sizes(mesh, vb_type, ib_type);

    if ((mesh->ab = MALLOC(sizeof(attribute_t) * vertex_count)) == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    mesh->vb = vertex_buffer;
    mesh->ib = index_buffer;
    mesh->vb_vertices = vertex_count;
    mesh->ib_indices = index_count;
    mesh->we_own_the_buffers = 0;

    init_attribute_buffer(mesh, vertex_count);
    calculate_aabb(mesh);

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
wsret
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       uintptr_t vertex_count, uintptr_t index_count,
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

    mesh->vb_vertices = vertex_count;
    mesh->ib_indices = index_count;
    mesh->we_own_the_buffers = 1;

    init_attribute_buffer(mesh, vertex_count);
    calculate_aabb(mesh);

    WSRET(WS_OK);

    ib_alloc_failed: FREE(mesh->vb);
    vb_alloc_failed: FREE(mesh->ab);
    ab_alloc_failed: WSRET(WS_ERR_OUT_OF_MEMORY);
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face_vertices(wsreal_t dst[9], const mesh_t* mesh, const wsib_t indices[3])
{
    mesh_get_face_vertices_from_buffer(dst, mesh->vb, indices, mesh->vb_type);
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face_indices(wsib_t dst[3], const mesh_t* mesh, uintptr_t face_index)
{
    mesh_get_face_indices_from_buffer(dst, mesh->ib, face_index, mesh->ib_type);
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face(face_t* dst, const mesh_t* mesh, uintptr_t face_index)
{
    mesh_get_face_from_buffers(dst, mesh->vb, mesh->ib, mesh->ab, face_index, mesh->vb_type, mesh->ib_type);
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face_vertices_from_buffer(wsreal_t dst[9], const void* vb, const wsib_t indices[3], mesh_vb_type_e vb_type)
{
#define READ_VEC3(dst, index, T) \
    (dst)[0] = (wsreal_t)*((T*)vb + (index) + 0); \
    (dst)[1] = (wsreal_t)*((T*)vb + (index) + 1); \
    (dst)[2] = (wsreal_t)*((T*)vb + (index) + 2);

    switch (vb_type)
    {
        case MESH_VB_FLOAT :
            READ_VEC3(dst + 0, indices[0]*3, float);
            READ_VEC3(dst + 3, indices[1]*3, float);
            READ_VEC3(dst + 6, indices[2]*3, float);
            break;
        case MESH_VB_DOUBLE :
            READ_VEC3(dst + 0, indices[0]*3, double);
            READ_VEC3(dst + 3, indices[1]*3, double);
            READ_VEC3(dst + 6, indices[2]*3, double);
            break;
        case MESH_VB_LONG_DOUBLE :
            READ_VEC3(dst + 0, indices[0]*3, long double);
            READ_VEC3(dst + 3, indices[1]*3, long double);
            READ_VEC3(dst + 6, indices[2]*3, long double);
            break;
    }
#undef READ_VEC3
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face_indices_from_buffer(wsib_t dst[3], const void* ib, uintptr_t face_index, mesh_ib_type_e ib_type)
{
    uintptr_t offset = face_index * 3;

#define CONVERT_IB(T) \
        dst[0] = (wsib_t)*((T*)ib + offset + 0); \
        dst[1] = (wsib_t)*((T*)ib + offset + 1); \
        dst[2] = (wsib_t)*((T*)ib + offset + 2)

    switch (ib_type)
    {
        case MESH_IB_INT8   : CONVERT_IB(int8_t);   break;
        case MESH_IB_UINT8  : CONVERT_IB(uint8_t);  break;
        case MESH_IB_INT16  : CONVERT_IB(int16_t);  break;
        case MESH_IB_UINT16 : CONVERT_IB(uint16_t); break;
        case MESH_IB_INT32  : CONVERT_IB(int32_t);  break;
        case MESH_IB_UINT32 : CONVERT_IB(uint32_t); break;
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
        case MESH_IB_INT64  : CONVERT_IB(int64_t);  break;
        case MESH_IB_UINT64 : CONVERT_IB(uint64_t); break;
#endif
    }
#undef CONVERT_IB
}

/* ------------------------------------------------------------------------- */
void
mesh_get_face_from_buffers(face_t* dst,
                           const void* vb, const void* ib, const attribute_t* attrs,
                           uintptr_t face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    wsib_t indices[3];
    union {
        vec3_t v3[3];
        wsreal_t arr[9];
    } vertices;

    mesh_get_face_indices_from_buffer(indices, ib, face_index, ib_type);
    mesh_get_face_vertices_from_buffer(vertices.arr, vb, indices, vb_type);

    *dst = face(
        vertex(vertices.v3[0], attrs[indices[0]]),
        vertex(vertices.v3[1], attrs[indices[1]]),
        vertex(vertices.v3[2], attrs[indices[2]])
    );
}

/* ------------------------------------------------------------------------- */
void
mesh_write_face_indices_to_buffer(void* ib, uintptr_t face_index, const wsib_t indices[3], mesh_ib_type_e ib_type)
{
    uintptr_t offset = face_index * 3;

#define WRITE_FACE(offset, indices, T) \
    *((T*)ib + (offset) + 0) = (T)(indices[0]); \
    *((T*)ib + (offset) + 1) = (T)(indices[1]); \
    *((T*)ib + (offset) + 2) = (T)(indices[2])

    switch (ib_type)
    {
        case MESH_IB_INT8   : WRITE_FACE(offset, indices, int8_t);   break;
        case MESH_IB_UINT8  : WRITE_FACE(offset, indices, uint8_t);  break;
        case MESH_IB_INT16  : WRITE_FACE(offset, indices, int16_t);  break;
        case MESH_IB_UINT16 : WRITE_FACE(offset, indices, uint16_t); break;
        case MESH_IB_INT32  : WRITE_FACE(offset, indices, int32_t);  break;
        case MESH_IB_UINT32 : WRITE_FACE(offset, indices, uint32_t); break;
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
        case MESH_IB_INT64  : WRITE_FACE(offset, indices, int64_t);  break;
        case MESH_IB_UINT64 : WRITE_FACE(offset, indices, uint64_t); break;
#endif
    }
#undef WRITE_FACE
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
static void init_attribute_buffer(mesh_t* mesh, uintptr_t vertex_count)
{
    uintptr_t i;
    for (i = 0; i != vertex_count; ++i)
        attribute_set_default_solid(&mesh->ab[i]);
}

/* ------------------------------------------------------------------------- */
static void calculate_aabb(mesh_t* mesh)
{
    uintptr_t f;
    mesh->aabb = aabb_reset();

    for (f = 0; f != mesh_face_count(mesh); ++f)
    {
        int v;
        wsib_t indices[3];
        wsreal_t vertices[9];
        mesh_get_face_indices(indices, mesh, f);
        mesh_get_face_vertices(vertices, mesh, indices);
        for (v = 0; v != 3; ++v)
        {
            int xyz;
            for (xyz = 0; xyz != 3; ++xyz)
            {
                if (vertices[v*3+xyz] < mesh->aabb.b.min.xyz[xyz])
                    mesh->aabb.b.min.xyz[xyz] = vertices[v*3+xyz];
                if (vertices[v*3+xyz] > mesh->aabb.b.max.xyz[xyz])
                    mesh->aabb.b.max.xyz[xyz] = vertices[v*3+xyz];
            }
        }
    }
}

/* ------------------------------------------------------------------------- */
int
mesh_is_manifold(const mesh_t* mesh)
{
    /* Condition is V + F - E = 2 */
    uintptr_t V, F, E, face_idx;
    hashmap_t hm;

    /* Have to count the number of unique edges in the mesh. */
    V = mesh_vertex_count(mesh);
    F = mesh_face_count(mesh);
    hashmap_construct(&hm, sizeof(wsib_t)*2, 0);
    /* Iterate 3 indices at a time (i.e. iterate faces)*/
    for (face_idx = 0; face_idx != mesh_face_count(mesh); ++face_idx)
    {
        wsib_t e1[2];
        wsib_t e2[2];
        wsib_t e3[2];
        wsib_t indices[3];

        mesh_get_face_indices(indices, mesh, face_idx);

        /* Need to sort the indices so "flipped" edges resolve to the same hash value */
        e1[0] = indices[0] < indices[1] ? indices[0] : indices[1];
        e1[1] = indices[0] > indices[1] ? indices[0] : indices[1];
        e2[0] = indices[1] < indices[2] ? indices[1] : indices[2];
        e2[1] = indices[1] > indices[2] ? indices[1] : indices[2];
        e3[0] = indices[2] < indices[0] ? indices[2] : indices[0];
        e3[1] = indices[2] > indices[0] ? indices[2] : indices[0];

        if (hashmap_insert(&hm, e1, NULL) == WS_ERR_OUT_OF_MEMORY)
            goto ran_out_of_memory;
        if (hashmap_insert(&hm, e2, NULL) == WS_ERR_OUT_OF_MEMORY)
            goto ran_out_of_memory;
        if (hashmap_insert(&hm, e3, NULL) == WS_ERR_OUT_OF_MEMORY)
            goto ran_out_of_memory;
    }

    E = hashmap_count(&hm);
    hashmap_destruct(&hm);

    if (V + F - E == 2)
        return 1;
    return 0;

    ran_out_of_memory : hashmap_destruct(&hm);
    return -1;
}

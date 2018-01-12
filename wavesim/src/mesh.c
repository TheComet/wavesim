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
    vector_construct(&mb->faces, sizeof(face_t));
    mb->aabb = aabb_reset();
    return mb;
}

/* ------------------------------------------------------------------------- */
void
mesh_builder_destroy(mesh_builder_t* mb)
{
    vector_clear_free(&mb->faces);
    FREE(mb);
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
        if ((vertex).position.data.axis < mb->aabb.data.a.data.axis) \
            mb->aabb.data.a.data.axis = (vertex).position.data.axis; \
        if ((vertex).position.data.axis > mb->aabb.data.b.data.axis) \
            mb->aabb.data.b.data.axis = (vertex).position.data.axis;
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
    mesh_vb_type_e vb_type;
    mesh_ib_type_e ib_type;
    int vb_size, ib_size;
    vector_t vb, ib, ab;
    mesh_t* mesh;

    /* Determine what datatype to use for the vertex buffer */
#if defined(WAVESIM_PRECISION_double)
    vb_type = MESH_VB_DOUBLE;
    vb_size = sizeof(double);
#elif defined(WAVESIM_PRECISION_float)
    vb_type = MESH_VB_FLOAT;
    vb_size = sizeof(float);
#else
    vb_type = MESH_VB_LONG_DOUBLE;
    vb_size = sizeof(long double);
#endif

    /* Determine what datatype to use for the index buffer; there will be
     * 3*faces indices */
    if (1 == 0) {}
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
    else if (vector_count(&mb->faces) * 3 >= (1<<32))
    {
        ib_type = MESH_IB_UINT64
        ib_size = 8;
    }
#endif
    else if (vector_count(&mb->faces) * 3 >= (1<<16))
    {
        ib_type = MESH_IB_UINT32;
        ib_size = 4;
    }
    else if (vector_count(&mb->faces) * 3 >= (1<<8))
    {
        ib_type = MESH_IB_UINT16;
        ib_size = 2;
    }
    else
    {
        ib_type = MESH_IB_UINT8;
        ib_size = 1;
    }

    vector_construct(&vb, vb_size);
    vector_construct(&ib, ib_size);
    vector_construct(&ab, sizeof(vertex_attr_t));

    /* Copy face vertices into buffers, avoiding duplicates */
    VECTOR_FOR_EACH(&mb->faces, face_t, face)
        unsigned int v;

        /*
         * If a duplicate vertex is found, its index is written to this array.
         */
        WS_IB duplicate_index[3] = {-1, -1, -1};
        for (v = 0; v != vector_count(&vb); v += 3)
        {
            int f;

            /* points to the attribute of this vertex triplet */
            vertex_attr_t* attr = vector_get_element(&ab, v/3);
            /* Construct a vertex object to make comparison easier */
            vertex_t vb_vertex = vertex(vec3(
                *(WS_REAL*)vector_get_element(&vb, v+0),
                *(WS_REAL*)vector_get_element(&vb, v+1),
                *(WS_REAL*)vector_get_element(&vb, v+2)),
                attr->reflection,
                attr->transmission,
                attr->absorbtion
            );

            /*
             * Compare each vertex in "face" against the current vertex in the
             * vertex buffer, "vb_vertex"
             */
            for (f = 0; f != 3; ++f)
                if (vertex_is_same(&face->vertices[f], &vb_vertex))
                    duplicate_index[f] = v / 3;

            if (duplicate_index[0] && duplicate_index[1] && duplicate_index[2])
                break;
        }

        /* Copy face into buffers */
        for (v = 0; v != 3; ++v)
        {
            if (duplicate_index[v] == (WS_IB)-1)
            {
                union {
                    uint8_t i8;
                    uint16_t i16;
                    uint32_t i32;
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
                    uint64_t i64;
#endif
                    WS_IB i;
                } index;
                if ((index.i = vector_push(&vb, &face->vertices[v].position.data.x)) == VECTOR_ERROR)
                    goto buffer_push_failed;
                if (         vector_push(&vb, &face->vertices[v].position.data.y) == VECTOR_ERROR)
                    goto buffer_push_failed;
                if (         vector_push(&vb, &face->vertices[v].position.data.z) == VECTOR_ERROR)
                    goto buffer_push_failed;
                if (         vector_push(&ab, &face->vertices[v].attr) == VECTOR_ERROR)
                    goto buffer_push_failed;

                index.i = index.i / 3;
                switch (ib_type)
                {
                    case MESH_IB_UINT8:
                        if (vector_push(&ib, &index.i8) == VECTOR_ERROR)
                            goto buffer_push_failed;
                        break;
                    case MESH_IB_UINT16:
                        if (vector_push(&ib, &index.i8) == VECTOR_ERROR)
                            goto buffer_push_failed;
                        break;
                    case MESH_IB_UINT32:
                        if (vector_push(&ib, &index.i8) == VECTOR_ERROR)
                            goto buffer_push_failed;
                        break;
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
                    case MESH_IB_UINT64:
                        if (vector_push(&ib, &index.i8) == VECTOR_ERROR)
                            goto buffer_push_failed;
                        break;
#endif
                    default: break;
                }
            }
            else
            {
                if (vector_push(&ib, &duplicate_index[v]) == VECTOR_ERROR)
                    goto buffer_push_failed;
            }
        }
    VECTOR_END_EACH

    /* Finally, create a mesh and pass buffers to it */
    mesh = mesh_create();
    if (mesh == NULL)
        goto alloc_mesh_failed;
    mesh_copy_from_buffers(mesh,
                           vb.data, ib.data,
                           vector_count(&vb), vector_count(&ib),
                           vb_type, ib_type);

    /* Copy attribute buffer into mesh */
    memcpy(mesh->ab, ab.data, sizeof(vertex_attr_t) * vector_count(&ab));

    vector_clear_free(&ab);
    vector_clear_free(&ib);
    vector_clear_free(&vb);

    return mesh;

    alloc_mesh_failed  :
    buffer_push_failed : vector_clear_free(&ab);
                         vector_clear_free(&ib);
                         vector_clear_free(&vb);
    OUT_OF_MEMORY(NULL);
}

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

    if ((mesh->ab = MALLOC(sizeof(vertex_attr_t) * vertex_count)) == NULL)
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

    if ((mesh->ab = MALLOC(sizeof(vertex_attr_t) * vertex_count)) == NULL)
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
mesh_get_vertex_position(mesh_t* mesh, int index)
{
    return mesh_get_vertex_position_from_buffer(mesh->vb, index, mesh->vb_type);
}

/* ------------------------------------------------------------------------- */
vec3_t
mesh_get_vertex_position_from_buffer(void* vb, int index, mesh_vb_type_e vb_type)
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
mesh_get_index_from_buffer(void* ib, int index, mesh_ib_type_e ib_type)
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
mesh_get_face_from_buffers(void* vb, void* ib, vertex_attr_t* attrs,
                           int face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type)
{
    WS_IB indices[3] = {
        mesh_get_index_from_buffer(ib, face_index * 3 + 0, ib_type),
        mesh_get_index_from_buffer(ib, face_index * 3 + 1, ib_type),
        mesh_get_index_from_buffer(ib, face_index * 3 + 2, ib_type)
    };
    vec3_t vertices[3] = {
        mesh_get_vertex_position_from_buffer(vb, indices[0], vb_type),
        mesh_get_vertex_position_from_buffer(vb, indices[1], vb_type),
        mesh_get_vertex_position_from_buffer(vb, indices[2], vb_type)
    };
    vertex_attr_t attributes[3] = {
        attrs[indices[0]],
        attrs[indices[1]],
        attrs[indices[2]]
    };
    return face(
        vertex(vertices[0], attributes[0].reflection, attributes[0].absorbtion, attributes[0].transmission),
        vertex(vertices[1], attributes[1].reflection, attributes[1].absorbtion, attributes[1].transmission),
        vertex(vertices[2], attributes[2].reflection, attributes[2].absorbtion, attributes[2].transmission)
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
        mesh->ab[i] = (vertex_attr_t){0, 0, 1}; /* full absorption by default */
}

/* ------------------------------------------------------------------------- */
static void calculate_aabb(mesh_t* mesh)
{
    int v, i;
    mesh->aabb = aabb_reset();

    for (v = 0; v != mesh->vb_count; ++v)
    {
        vec3_t pos = mesh_get_vertex_position(mesh, v);
        for (i = 0; i != 3; ++i)
        {
            if (pos.xyz[i] < mesh->aabb.data.a.xyz[i])
                mesh->aabb.data.a.xyz[i] = pos.xyz[i];
            if (pos.xyz[i] > mesh->aabb.data.b.xyz[i])
                mesh->aabb.data.b.xyz[i] = pos.xyz[i];
        }
    }
}

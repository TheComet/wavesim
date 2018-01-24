#include "wavesim/face.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/mesh_builder.h"
#include <string.h>

/* ------------------------------------------------------------------------- */
wsret
mesh_builder_create(mesh_builder_t** mb)
{
    *mb = MALLOC(sizeof(**mb));
    if (mb == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    vector_construct(&(*mb)->faces, sizeof(face_t));
    (*mb)->aabb = aabb_reset();
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
mesh_builder_destroy(mesh_builder_t* mb)
{
    vector_clear_free(&mb->faces);
    FREE(mb);
}

/* ------------------------------------------------------------------------- */
wsret
mesh_builder_add_face(mesh_builder_t* mb, face_t face)
{
    face_t* face_store = vector_emplace(&mb->faces);
    if (face_store == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    *face_store = face;

    /* Update bounding box, maybe the new face has made it grow */
#define UPDATE(vertex, axis) \
        if ((vertex).position.v.axis < mb->aabb.b.min.v.axis) \
            mb->aabb.b.min.v.axis = (vertex).position.v.axis; \
        if ((vertex).position.v.axis > mb->aabb.b.max.v.axis) \
            mb->aabb.b.max.v.axis = (vertex).position.v.axis;
    UPDATE(face.vertices[0], x); UPDATE(face.vertices[0], y); UPDATE(face.vertices[0], z);
    UPDATE(face.vertices[1], x); UPDATE(face.vertices[1], y); UPDATE(face.vertices[1], z);
    UPDATE(face.vertices[2], x); UPDATE(face.vertices[2], y); UPDATE(face.vertices[2], z);
#undef UPDATE

    return 0;
}

/* ------------------------------------------------------------------------- */
wsret
mesh_builder_build(mesh_t** mesh, mesh_builder_t* mb)
{
    mesh_vb_type_e vb_type;
    mesh_ib_type_e ib_type;
    int vb_size, ib_size;
    vector_t vb, ib, ab;

    /* Determine what datatype to use for the vertex buffer */
#if defined(WS_PRECISION_double)
    vb_type = MESH_VB_DOUBLE;
    vb_size = sizeof(double);
#elif defined(WS_PRECISION_float)
    vb_type = MESH_VB_FLOAT;
    vb_size = sizeof(float);
#else
    vb_type = MESH_VB_LONG_DOUBLE;
    vb_size = sizeof(long double);
#endif

    /* Determine what datatype to use for the index buffer; there will be
     * 3*faces indices */
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
    if (vector_count(&mb->faces) * 3 >= (1<<32))
    {
        ib_type = MESH_IB_UINT64
        ib_size = 8;
    }
    else
#endif
    if (vector_count(&mb->faces) * 3 >= (1<<16))
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
    vector_construct(&ab, sizeof(attribute_t));

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
            attribute_t* attr = vector_get_element(&ab, v/3);
            /* Construct a vertex object to make comparison easier */
            vertex_t vb_vertex = vertex(vec3(
                *(WS_REAL*)vector_get_element(&vb, v+0),
                *(WS_REAL*)vector_get_element(&vb, v+1),
                *(WS_REAL*)vector_get_element(&vb, v+2)),
                *attr
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
                if ((index.i = (WS_IB)vector_push(&vb, &face->vertices[v].position.v.x)) == VECTOR_ERROR)
                    goto buffer_push_failed;
                if (         vector_push(&vb, &face->vertices[v].position.v.y) == VECTOR_ERROR)
                    goto buffer_push_failed;
                if (         vector_push(&vb, &face->vertices[v].position.v.z) == VECTOR_ERROR)
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
    if (mesh_create(mesh) != WS_OK)
        goto alloc_mesh_failed;
    if (mesh_copy_from_buffers(*mesh,
                               vb.data, ib.data,
                               (WS_IB)vector_count(&vb) / 3, (WS_IB)vector_count(&ib),
                               vb_type, ib_type) != WS_OK)
        goto copy_buffers_failed;

    /* Copy attribute buffer into mesh */
    memcpy((*mesh)->ab, ab.data, sizeof(attribute_t) * vector_count(&ab));

    vector_clear_free(&ab);
    vector_clear_free(&ib);
    vector_clear_free(&vb);

    return WS_OK;

    copy_buffers_failed : mesh_destroy(*mesh);
    alloc_mesh_failed   : *mesh = NULL;
    buffer_push_failed  : vector_clear_free(&ab);
                          vector_clear_free(&ib);
                          vector_clear_free(&vb);
    return WS_ERR_OUT_OF_MEMORY;
}

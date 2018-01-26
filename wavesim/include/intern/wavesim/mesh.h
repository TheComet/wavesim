#ifndef MESH_H
#define MESH_H

#include "wavesim/config.h"
#include "wavesim/aabb.h"
#include "wavesim/vector.h"
#include "wavesim/face.h"

C_BEGIN

typedef enum mesh_ib_type_e
{
    MESH_IB_INT8 = 0,
    MESH_IB_UINT8,
    MESH_IB_INT16,
    MESH_IB_UINT16,
    MESH_IB_INT32,
    MESH_IB_UINT32
#ifdef WAVESIM_64BIT_INDEX_BUFFERS
    ,MESH_IB_INT64,
    MESH_IB_UINT64
#endif
} mesh_ib_type_e;

typedef enum mesh_vb_type_e
{
    MESH_VB_FLOAT = 0,
    MESH_VB_DOUBLE,
    MESH_VB_LONG_DOUBLE
} mesh_vb_type_e;

typedef struct mesh_t
{
    attribute_t*     ab;       /* attribute buffer, will be same size as vb */
    void*            vb;       /* vertex buffer */
    void*            ib;       /* index buffer */

    mesh_vb_type_e   vb_type;
    mesh_ib_type_e   ib_type;
    WS_IB            vb_count;  /* Number of vertices in the buffer. This
                                 * number is always divisible by 3, because the
                                 * vertices appear as float triplets in the
                                 * buffer. As such, the actual size of the
                                 * buffer is 3 times larger than this value. */
    WS_IB            ib_count;  /* Number of indices in the buffer */
    WS_IB            vb_size;   /* Size in bytes of one element in the vertex
                                 * buffer. vb_size*3 will be the size in bytes
                                 * of one vertex triplet */
    WS_IB            ib_size;   /* Size in bytes of one index buffer element */

    aabb_t           aabb;

    char             we_own_the_buffers;
} mesh_t;

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
mesh_create(mesh_t** mesh);

WAVESIM_PRIVATE_API void
mesh_construct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destruct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destroy(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_clear_buffers(mesh_t* mesh);

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
mesh_assign_buffers(mesh_t* mesh,
                    void* vertex_buffer, void* index_buffer,
                    WS_IB vertex_count, WS_IB index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       WS_IB vertex_count, WS_IB index_count,
                       mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API vec3_t
mesh_get_vertex_position(mesh_t* mesh, WS_IB index);

WAVESIM_PRIVATE_API vec3_t
mesh_get_vertex_position_from_buffer(void* vb, WS_IB index, mesh_vb_type_e vb_type);

WAVESIM_PRIVATE_API WS_IB
mesh_get_index_from_buffer(void* ib, WS_IB index, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API face_t
mesh_get_face(const mesh_t* mesh, WS_IB face_index);

WAVESIM_PRIVATE_API face_t
mesh_get_face_from_buffers(void* vb, void* ib, attribute_t* attrs,
                           WS_IB face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

#define mesh_index_count(mesh) (mesh->ib_count)
#define mesh_vertex_count(mesh) (mesh->vb_count)
#define mesh_face_count(mesh) (mesh->ib_count/3)

C_END

#endif /* MESH_H */

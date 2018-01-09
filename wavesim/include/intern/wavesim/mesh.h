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
    MESH_IB_UINT32,
    MESH_IB_INT64,
    MESH_IB_UINT64
} mesh_ib_type_e;

typedef enum mesh_vb_type_e
{
    MESH_VB_FLOAT = 0,
    MESH_VB_DOUBLE,
    MESH_VB_LONG_DOUBLE
} mesh_vb_type_e;

typedef struct mesh_builder_t
{
    vector_t  faces;  /* holds face_t instances */
    aabb_t    aabb;
} mesh_builder_t;

typedef struct mesh_t
{
    vertex_attrs_t*    ab;       /* attribute buffer, will be same size as vb */
    void*            vb;       /* vertex buffer */
    void*            ib;       /* index buffer */

    mesh_vb_type_e   vb_type;
    mesh_ib_type_e   ib_type;
    int              vb_count;
    int              ib_count;
    int              vertex_size;
    int              index_size;

    aabb_t           aabb;

    char             we_own_the_buffers;
} mesh_t;

/* ---- Mesh Builder ---- */

WAVESIM_PRIVATE_API mesh_builder_t*
mesh_builder_create(void);

WAVESIM_PRIVATE_API int
mesh_builder_add_face(mesh_builder_t* mb, face_t face);

WAVESIM_PRIVATE_API mesh_t*
mesh_builder_finalize(mesh_builder_t* mb);

WAVESIM_PRIVATE_API void
mesh_builder_destroy(mesh_builder_t* mb);

/* ---- Mesh ---- */

WAVESIM_PRIVATE_API mesh_t*
mesh_create(void);

WAVESIM_PRIVATE_API void
mesh_construct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destruct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destroy(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_clear_buffers(mesh_t* mesh);

WAVESIM_PRIVATE_API int
mesh_assign_buffers(mesh_t* mesh,
                    void* vertex_buffer, void* index_buffer,
                    int vertex_count, int index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API int
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       int vertex_count, int index_count,
                       mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API vec3_t
mesh_get_vertex_position(mesh_t* mesh, int index);

#define mesh_face_count(mesh) vector_count(mesh->ib_count)

C_END

#endif /* MESH_H */

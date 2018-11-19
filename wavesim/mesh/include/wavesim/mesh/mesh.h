#ifndef MESH_H
#define MESH_H

#include "wavesim/config.h"
#include "wavesim/aabb.h"
#include "wavesim/vector.h"
#include "wavesim/mesh/face.h"

/*
 * Depending on how wavesim was configured, determine the default type that
 * should be used for the index buffer (MESH_IB_DEFAULT) and for the vertex
 * buffer (MESH_VB_DEFAULT).
 */
#if defined(WAVESIM_64BIT_INDEX_BUFFERS)
#   define MESH_IB_DEFAULT MESH_IB_UINT64
#else
#   define MESH_IB_DEFAULT MESH_IB_UINT32
#endif

#if defined(WAVESIM_PRECISION_LONG_DOUBLE)
#   define MESH_VB_DEFAULT MESH_VB_LONG_DOUBLE
#elif defined(WAVESIM_PRECISION_DOUBLE)
#   define MESH_VB_DEFAULT MESH_VB_DOUBLE
#elif defined(WAVESIM_PRECISION_FLOAT)
#   define MESH_VB_DEFAULT MESH_VB_FLOAT
#else
#   error Unknown precision.
#endif

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
    /* Usually the filename. It does not have to be globally unique */
    char*            name;
    /* Attribute buffer, will be the same size as vb */
    attribute_t*     ab;
    /* Vertex buffer, an array of float/double/long double triplets. The length
     * of this buffer is always divisible by 3 because each vertex is composed
     * of an xyz position */
    void*            vb;
    /* Index buffer, an array of integers (type varies) that store offsets into
     * the vertex buffer. The length is always divisible by 3, because the mesh
     * is always composed of triangular faces. */
    void*            ib;
    /* What datatype is stored in vb */
    mesh_vb_type_e   vb_type;
    /* What datatype is stored in ib */
    mesh_ib_type_e   ib_type;
    /* Total number of vertices in the buffer. The actual size (in bytes) of
     * the vertex buffer is 3 times larger than this value. */
    uintptr_t           vb_vertices;
    /* Number of indices in the buffer */
    uintptr_t           ib_indices;
    /* Size in bytes of one element in the vertex buffer. vb_size*3 will be the
     * size in bytes of one vertex triplet */
    uint8_t          vb_size;
    /* Size in bytes of one index buffer element */
    uint8_t          ib_size;
    /* Mesh axis-aligned bounding box */
    aabb_t           aabb;
    /* This is zero if we are to not free() the index and vertex buffers. The
     * attribute buffer we always own */
    char             we_own_the_buffers;
} mesh_t;

WAVESIM_PRIVATE_API wsret WAVESIM_WARN_UNUSED
mesh_create(mesh_t** mesh, const char* name);

WAVESIM_PRIVATE_API wsret WAVESIM_WARN_UNUSED
mesh_construct(mesh_t* mesh, const char* name);

WAVESIM_PRIVATE_API void
mesh_destruct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destroy(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_clear_buffers(mesh_t* mesh);

WAVESIM_PRIVATE_API wsret WAVESIM_WARN_UNUSED
mesh_assign_buffers(mesh_t* mesh,
                    void* vertex_buffer, void* index_buffer,
                    uintptr_t vertex_count, uintptr_t index_count,
                    mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API wsret WAVESIM_WARN_UNUSED
mesh_copy_from_buffers(mesh_t* mesh,
                       const void* vertex_buffer, const void* index_buffer,
                       uintptr_t vertex_count, uintptr_t index_count,
                       mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API void
mesh_get_face_vertices(wsreal_t dst[9], const mesh_t* mesh, const wsib_t indices[3]);

WAVESIM_PRIVATE_API void
mesh_get_face_indices(wsib_t dst[3], const mesh_t* mesh, uintptr_t face_index);

WAVESIM_PRIVATE_API void
mesh_get_face(face_t* dst, const mesh_t* mesh, uintptr_t face_index);

WAVESIM_PRIVATE_API void
mesh_get_face_vertices_from_buffer(wsreal_t dst[9], const void* vb, const wsib_t indices[3], mesh_vb_type_e vb_type);

WAVESIM_PRIVATE_API void
mesh_get_face_indices_from_buffer(wsib_t dst[3], const void* ib, uintptr_t face_index, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API void
mesh_get_face_from_buffers(face_t* dst,
                           const void* vb, const void* ib, const attribute_t* attrs,
                           uintptr_t face_index,
                           mesh_vb_type_e vb_type, mesh_ib_type_e ib_type);

WAVESIM_PRIVATE_API void
mesh_write_face_indices_to_buffer(void* ib, uintptr_t face_index, const wsib_t indices[3], mesh_ib_type_e ib_type);

#define mesh_index_count(mesh) (mesh->ib_indices)
#define mesh_vertex_count(mesh) (mesh->vb_vertices)
#define mesh_face_count(mesh) (mesh->ib_indices/3)

WAVESIM_PRIVATE_API int
mesh_is_manifold(const mesh_t* mesh);

C_END

#endif /* MESH_H */

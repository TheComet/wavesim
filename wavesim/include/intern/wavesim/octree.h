#ifndef OCTREE_H
#define OCTREE_H

#include "wavesim/config.h"
#include "wavesim/vector.h"
#include "wavesim/aabb.h"

C_BEGIN

typedef struct mesh_t mesh_t;

typedef struct octree_t
{
    struct octree_t* children;
    struct octree_t* parent;
    mesh_t*          mesh;
    aabb_t           aabb;
    vector_t         index_buffer;
} octree_t;

WAVESIM_PRIVATE_API octree_t*
octree_create(void);

WAVESIM_PRIVATE_API void
octree_destroy(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_construct(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_destruct(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_clear(octree_t* octree);

WAVESIM_PRIVATE_API int
octree_subdivide(octree_t* octree);

#define octree_face(octree, idx) \
    *(face_t**)vector_get_element(&octree->faces, idx)

#define octree_face_count(octree) \
    octree->mesh->ib_count

WAVESIM_PRIVATE_API intptr_t
octree_build_from_mesh(octree_t* octree, const mesh_t* mesh, vec3_t smallest_subdivision);

WAVESIM_PRIVATE_API const vector_t*
octree_intersect_aabb(const octree_t* octree, aabb_t aabb);

C_END

#endif /* OCTREE_H */

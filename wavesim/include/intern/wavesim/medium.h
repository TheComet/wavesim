#ifndef PARTITION_H
#define PARTITION_H

#include "wavesim/config.h"
#include "wavesim/vector.h"
#include "wavesim/aabb.h"

C_BEGIN

typedef struct mesh_t mesh_t;
typedef struct medium_t medium_t;
typedef struct octree_t octree_t;
typedef wsret (*medium_decomposition_func)(medium_t*, const octree_t*, const medium_t*);

typedef struct medium_t
{
    aabb_t                       boundary;
    vec3_t                       grid_size;
    vector_t                     areas; /* medium_area_t */
    medium_decomposition_func decompose;
} medium_t;

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
medium_create(medium_t** partition);

WAVESIM_PRIVATE_API void
medium_destroy(medium_t* partition);

WAVESIM_PRIVATE_API void
medium_construct(medium_t* partition);

WAVESIM_PRIVATE_API void
medium_destruct(medium_t* partition);

WAVESIM_PRIVATE_API void
medium_clear(medium_t* partition);

WAVESIM_PRIVATE_API int
medium_add_area(medium_t* partition, const WS_REAL bounding_box[6], WS_REAL sound_speed);

WAVESIM_PRIVATE_API void
medium_set_decomposition_method(medium_t* partition,
                                medium_decomposition_func method);

WAVESIM_PRIVATE_API wsret
medium_decompose_systematic(medium_t* partition,
                            const octree_t* octree,
                            const medium_t* medium);

WAVESIM_PRIVATE_API wsret
medium_decompose_greedy_random(medium_t* partition,
                               const octree_t* octree,
                               const medium_t* medium);

WAVESIM_PRIVATE_API wsret
medium_build_from_mesh(medium_t* partition,
                       const medium_t* medium,
                       const mesh_t* mesh,
                       const WS_REAL grid_size[3]);

C_END

#endif /* PARTITION_H */

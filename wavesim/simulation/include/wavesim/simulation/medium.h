#ifndef PARTITION_H
#define PARTITION_H

#include "wavesim/config.h"
#include "wavesim/aabb.h"
#include "wavesim/return_codes.h"
#include "wavesim/vector.h"
#include "wavesim/mesh/attribute.h"

C_BEGIN

typedef struct mesh_t mesh_t;
typedef struct medium_t medium_t;
typedef struct octree_t octree_t;
typedef wsret (*medium_decomposition_func)(medium_t*, const octree_t*, const medium_t*,const wsreal_t[3]);

typedef struct medium_t
{
    aabb_t                       boundary;
    vector_t                     partitions; /* medium_partition_t */
    medium_decomposition_func    decompose;
} medium_t;

typedef struct medium_partition_t
{
    aabb_t aabb;
    attribute_t attr;
    wsreal_t cell_size;           /* Calculated using simulation->max_frequency */
    wsreal_t time_step;           /* Calculated using simulation->max_frequency */
    uintptr_t cell_count[3];      /* Calculated using simulation->max_frequency */
    vector_t adjacent_partitions; /* int32_t (indices into medium->partitions) */
} medium_partition_t;

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
medium_create(medium_t** medium);

WAVESIM_PRIVATE_API void
medium_destroy(medium_t* medium);

WAVESIM_PRIVATE_API void
medium_construct(medium_t* medium);

WAVESIM_PRIVATE_API void
medium_destruct(medium_t* medium);

WAVESIM_PRIVATE_API void
medium_clear(medium_t* medium);

WAVESIM_PRIVATE_API int
medium_add_partition(medium_t* medium, const wsreal_t bounding_box[6], attribute_t attr);

WAVESIM_PRIVATE_API void
medium_set_decomposition_method(medium_t* medium,
                                medium_decomposition_func method);

WAVESIM_PRIVATE_API wsret
medium_decompose_systematic(medium_t* medium,
                            const octree_t* octree,
                            const medium_t* mediumdef,
                            const wsreal_t grid_size[3]);

WAVESIM_PRIVATE_API wsret
medium_decompose_greedy_random(medium_t* medium,
                               const octree_t* octree,
                               const medium_t* mediumdef,
                               const wsreal_t grid_size[3]);

WAVESIM_PRIVATE_API wsret
medium_build_from_mesh(medium_t* medium,
                       const medium_t* mediumdef,
                       const mesh_t* mesh,
                       const wsreal_t grid_size[3]);

/*!
 * @brief This needs to be called before a simulation can be made. Calculates
 * the partition's spatial discretization (cell_size, cell_dims) as well as the
 * required minimum time step (time_step).
 *
 * @param[in] max_frequency Essentially, the lower the maximum frequency, the
 * more coarse the cell and time step can be, resulting in less memory
 * consumption and faster simulation times. This comes at the cost of not
 * simulating any frequencies above the specified maximum frequency.
 *
 * The minimum cell size "h" relies on the Nyquist theorem, which requires that
 *
 *        l(max)        c
 *   h <= ------   = --------,  where l(max)=wavelength, c=sound velocity, v=frequency
 *           2       2*v(max)
 *
 * Courant–Friedrichs–Lewy (CFL) condition requires that the minimum time step
 * be:
 *            h
 *   dt < ---------
 *        c*sqrt(3)
 *
 * @param[in] cell_tolerance A value between 0.001 and 1.0. Numbers outside
 * of this range will be clamped and a warning will be written to the log.
 * Specifies the percentage a cell is allowed to be misaligned with the partition.
 *
 * Because the partitions are not necessarily cubes, but the cells within those
 * partitions *must* be cubes, the initially calculated cell size will not
 * align properly with the partition. The cell size will therefore be decreased
 * until a point is found where all cells align within the specified tolerance.
 * Choosing small values can potentially cause the final cell size to be
 * hundreds of times smaller than necessary, whereas choosing a very large
 * tolerance will result in "spatial discontinuities" at interfaces. A good
 * value to start with is 0.1. A value of 1.0 means you don't care about
 * alignment.
 */
WAVESIM_PRIVATE_API void
medium_set_resolution(medium_t* medium, wsreal_t max_frequency, wsreal_t cell_tolerance);

WAVESIM_PRIVATE_API uintptr_t
medium_cell_count(medium_t* medium);

#define medium_partition_count(medium) \
        vector_count(&(medium)->partitions)

#define medium_get_partition(medium, partition_idx) \
        (medium_partition_t*)vector_get_element(&medium->partitions, partition_idx)

C_END

#endif /* PARTITION_H */

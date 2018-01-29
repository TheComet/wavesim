#ifndef OCTREE_H
#define OCTREE_H

#include "wavesim/config.h"
#include "wavesim/vector.h"
#include "wavesim/aabb.h"

C_BEGIN

typedef struct mesh_t mesh_t;

typedef struct octree_node_t
{
    struct octree_node_t* children;
    struct octree_node_t* parent;
    aabb_t                aabb;
    vector_t              index_buffer;
} octree_node_t;

typedef struct octree_t
{
    const mesh_t*    mesh;
    octree_node_t    root;
} octree_t;

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
octree_create(octree_t** octree);

WAVESIM_PRIVATE_API void
octree_destroy(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_construct(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_destruct(octree_t* octree);

WAVESIM_PRIVATE_API void
octree_clear(octree_t* octree);

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
octree_subdivide(octree_t* octree, octree_node_t* node);

#define octree_face(octree, idx) \
    *(face_t**)vector_get_element(&octree->faces, idx)

#define octree_face_count(octree) \
    octree->mesh->ib_count / 3

WAVESIM_PRIVATE_API wsret WS_WARN_UNUSED
octree_build_from_mesh(octree_t* octree, const mesh_t* mesh, int max_depth);

/*!
 * @brief Queries the octree for which faces intersect the specified bounding
 * box.
 * @param[in] octree The octree instance.
 * @param[out] result A list of indices describing the faces that *may*
 * intersect the specified bounding box are pushed into this vector. "May"
 * because the test is a simple AABB test, not a proper intersection test. The
 * faces need to be further evaluated to check whether they actually intersect
 * or not. The type of data in the the vector is identical to the mesh's index
 * buffer that was used to build the octree. In other words, in order to get
 * index data out of this list, use:
 *
 *   mesh_get_index(retval->data, index, mesh->ib_type);
 *
 * Note that the returned list can be empty.
 * @param[in] aabb The area in the octree to query.
 * @return Returns 0 on success. -1 if an error occurred. 1 if the query was
 * outside of the octree boundary.
 */
WAVESIM_PRIVATE_API int
octree_query_potential_faces(const octree_t* octree, vector_t* result, const double aabb[6]);

/*!
 * @brief Checks whether a point is located inside or outside of the 3D mesh.
 * @note The mesh must be a closed mesh (i.e. no holes) for this check to have
 * any meaning.
 */
WAVESIM_PRIVATE_API int
octree_query_point_is_inside_mesh(const octree_t* octree, const wsreal_t p[3]);

C_END

#endif /* OCTREE_H */

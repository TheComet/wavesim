#include "wavesim/hashmap.h"
#include "wavesim/hash.h"
#include "wavesim/log.h"
#include "wavesim/memory.h"
#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/octree.h"
#include "wavesim/mesh/intersections.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define IDX(x, y, z) \
    x*4 + y*2 + z

#define CX(idx) \
    (idx >= 4 ? 1 : 0)

#define CY(idx) \
    ((idx >= 2 && idx < 4) || idx >= 6 ? 1 : 0)

#define CZ(idx) \
    (idx % 2 ? 1 : 0)

/* ------------------------------------------------------------------------- */
wsret
octree_create(octree_t** octree)
{
    *octree = MALLOC(sizeof **octree);
    if (*octree == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    octree_construct(*octree);
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
octree_construct(octree_t* octree)
{
    octree->mesh = NULL;
    octree->root.parent = NULL;
    octree->root.children = NULL;
    octree->root.index_buffer.data = NULL;
    octree->root.index_buffer.count = 0;
    octree->root.aabb = aabb_reset();
}

/* ------------------------------------------------------------------------- */
void
octree_destruct(octree_t* octree)
{
    octree_clear(octree);
}

/* ------------------------------------------------------------------------- */
void
octree_destroy(octree_t* octree)
{
    octree_destruct(octree);
    FREE(octree);
}

/* ------------------------------------------------------------------------- */
void
node_destroy_children(octree_node_t* node)
{
    vector_clear_free(&node->index_buffer);

    if (node->children != NULL)
    {
        int i;
        for (i = 0; i != 8; ++i)
            node_destroy_children(&node->children[i]);
        FREE(node->children);
        node->children = NULL;
    }
}

/* ------------------------------------------------------------------------- */
wsret
node_create_children(octree_node_t** children, octree_node_t* parent, const mesh_t* mesh)
{
    int i;
    aabb_t bb_parent;
    vec3_t bb_dims;

    *children = MALLOC(sizeof(octree_node_t) * 8);
    if (children == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    /* Initialize children fields */
    for (i = 0; i != 8; ++i)
    {
        (*children)[i].children = NULL;
        (*children)[i].parent = parent;
        vector_construct(&(*children)[i].index_buffer, mesh->ib_size);
    }

    /* Calculate child bounding boxes */
    bb_parent = (parent == NULL ? mesh->aabb : parent->aabb);
    bb_dims = AABB_DIMS(bb_parent);
    for (i = 0; i != 8; ++i)
    {
        AABB_AX((*children)[i].aabb) = AABB_AX(bb_parent) + (CX(i) + 0) * bb_dims.v.x * 0.5;
        AABB_BX((*children)[i].aabb) = AABB_AX(bb_parent) + (CX(i) + 1) * bb_dims.v.x * 0.5;
        AABB_AY((*children)[i].aabb) = AABB_AY(bb_parent) + (CY(i) + 0) * bb_dims.v.y * 0.5;
        AABB_BY((*children)[i].aabb) = AABB_AY(bb_parent) + (CY(i) + 1) * bb_dims.v.y * 0.5;
        AABB_AZ((*children)[i].aabb) = AABB_AZ(bb_parent) + (CZ(i) + 0) * bb_dims.v.z * 0.5;
        AABB_BZ((*children)[i].aabb) = AABB_AZ(bb_parent) + (CZ(i) + 1) * bb_dims.v.z * 0.5;
    }

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
octree_clear(octree_t* octree)
{
    /* destroy all children and reset the root node */
    node_destroy_children(&octree->root);

    /* release reference to mesh object */
    octree->mesh = NULL;
}

/* ------------------------------------------------------------------------- */
wsret
octree_subdivide(octree_t* octree, octree_node_t* node)
{
    /* Subdivide is only valid for leaf nodes */
    if (node->children != NULL)
        return WS_ERR_SUBDIVIDED_NON_LEAF_NODE;

    return node_create_children(&node->children, node, octree->mesh);
}

/* ------------------------------------------------------------------------- */
static int
determine_split(octree_node_t* node, const wsreal_t bb[6])
{
    int x, y, z;

    /* Calculate the split plane locations */
    vec3_t split_location = AABB_DIMS(node->aabb);
    vec3_mul_scalar(split_location.xyz, 0.5);
    vec3_add_vec3(split_location.xyz, node->aabb.b.min.xyz);

    /*
     * For there to be a successful split, the bounding box must fall
     * definitively into one of the eight octants. If it intersects any of the
     * split planes, a split cannot be performed.
     *
     * Furthermore: For the split to be unambiguous, the checks must be
     *     min >= split
     *     max <  split  <-- no equals
     */
    if (bb[3] < split_location.v.x) x = 0;
    else if (bb[0] >= split_location.v.x) x = 1;
    else return -1;

    if (bb[4] < split_location.v.y) y = 0;
    else if (bb[1] >= split_location.v.y) y = 1;
    else return -1;

    if (bb[5] < split_location.v.z) z = 0;
    else if (bb[2] >= split_location.v.z) z = 1;
    else return -1;

    return IDX(x, y, z);
}

/* ------------------------------------------------------------------------- */
static wsret
add_face_to_octree_recursive(octree_t* octree,
                             octree_node_t* node,
                             const wsreal_t face_bb[6],
                             const wsib_t face_indices[3],
                             int max_depth)
{
    wsret result;
    int split_idx;
    void* ib_dest;

    if (max_depth != 0 && (split_idx = determine_split(node, face_bb)) != -1)
    {
        /* Check if node needs subdividing */
        if (node->children == NULL)
            if ((result = octree_subdivide(octree, node)) != WS_OK)
                return result;

        return add_face_to_octree_recursive(octree,
                                            &node->children[split_idx],
                                            face_bb,
                                            face_indices,
                                            max_depth - 1);
    }

    /*
     * We don't know the integer types that get written to the octree node
     * index buffer (they may not be a wsib_t type). All we can do is reserve
     * space for 3 additional indices in the node's vector and get an offset to
     * the first element to write the indices using the proper mesh_* routine.
     */
    if ((ib_dest = vector_emplace_multi(&node->index_buffer, 3)) == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    mesh_write_face_indices_to_buffer(ib_dest, 0, face_indices, octree->mesh->ib_type);

    return WS_OK;
}
static wsret
add_face_to_octree(octree_t* octree, const wsreal_t face_bb[6], const wsib_t face_indices[3], int max_depth)
{
    return add_face_to_octree_recursive(octree, &octree->root, face_bb, face_indices, max_depth);
}
wsret
octree_build_from_mesh(octree_t* octree, const mesh_t* mesh, int max_depth)
{
    uintptr_t face_idx;
    wsret result;

    /* Clear old octree if it exists */
    octree_clear(octree);

    /* Initialize root child, which is in-place in the octree structure */
    octree->mesh = mesh;
    octree->root.aabb = mesh->aabb;
    vector_construct(&octree->root.index_buffer, mesh->ib_size);

    for (face_idx = 0; face_idx != mesh_face_count(mesh); ++face_idx)
    {
        /* Look up indices and vertices for the current face */
        wsib_t indices[3];
        wsreal_t vertices[9];
        mesh_get_face_indices(indices, mesh, face_idx);
        mesh_get_face_vertices(vertices, mesh, indices);

        /* Calculate face AABB */
        aabb_t face_bb = aabb_from_3_points(vertices+0, vertices+3, vertices+6);

        /* Add this face to the octree */
        if ((result = add_face_to_octree(octree, face_bb.xyzxyz, indices, max_depth)) != WS_OK)
            goto adding_face_to_octree_failed;
    }

    return WS_OK;

    adding_face_to_octree_failed : octree_clear(octree);
    return result;
}

/* ------------------------------------------------------------------------- */
static int
octree_query_potential_faces_recursive(const octree_node_t* node, vector_t* result, const wsreal_t bb[6])
{
    /* This node and all children are of no interest */
    if (intersect_aabb_aabb_test(node->aabb.xyzxyz, bb) == 0)
        return 1;

    /*
     * We want to keep drilling deeper until the node size is smaller or equal
     * to the specified bounding box. Going any deeper is useless because all
     * children beyond that point will intersect anyway, and going shallower is
     * less efficient because we'd have to do more index buffer copies.
     */
    if (node->children != NULL)
    {
        vec3_t node_dims = AABB_DIMS(node->aabb);
        if (node_dims.v.x > bb[3] - bb[0] ||
            node_dims.v.y > bb[4] - bb[1] ||
            node_dims.v.z > bb[5] - bb[2])
        {
            int i, node_counter = 0;
            for (i = 0; i != 8; ++i)
            {
                int child_result = octree_query_potential_faces_recursive(&node->children[i], result, bb);
                if (child_result == -1)
                    return child_result;
                node_counter += child_result;
            }
            return node_counter;
        }
    }

    /*
     * We're at the bottom, add this node's indices to the result list and
     * avoid duplicate faces. In the worst case, we have to do a linear search
     * for indices 7 times (if the bounding box happens to intersect all 8
     * neighbouring nodes).
     *
     * Note that because we don't know the type of data in the result vector
     * or in the node vector, we have to do a direct memory comparison of
     * 3 element chunks.
     *
     * TODO: Maybe profile this, depending on how complex the mesh is it might
     * matter.
     */
    {
        uintptr_t node_chunk_idx;
        const vector_t* vn = &node->index_buffer;
        uintptr_t chunk_size = vn->element_size * 3;
        uintptr_t node_chunk_count = vector_count(vn) / 3;
        uintptr_t result_chunk_count = vector_count(result) / 3;

        assert(vector_count(vn) % 3 == 0);
        assert(vector_count(result) % 3 == 0);

        /* For all index triplets in the node... */
        for (node_chunk_idx = 0; node_chunk_idx != node_chunk_count; node_chunk_idx++)
        {
            /* Try to find the current triplet in the result index buffer */
            uintptr_t result_chunk_idx = 0;
            for (result_chunk_idx = 0; result_chunk_idx != result_chunk_count; result_chunk_idx++)
            {
                if (memcmp(&vn->data[node_chunk_idx*chunk_size],
                           &result->data[result_chunk_idx*chunk_size],
                           chunk_size) != 0)
                    goto identical_face_found;
            }

            /*
             * Duplicate indices not found, append this chunk to the vector by
             * emplacing 3 times.
             */
            if (vector_emplace(result) == NULL) return -1;
            if (vector_emplace(result) == NULL) return -1;
            if (vector_emplace(result) == NULL) return -1;
            memcpy(result->data + (result->count * result->element_size) - chunk_size,
                   &vn->data[node_chunk_idx*chunk_size],
                   chunk_size);

            identical_face_found: continue;
        }
    }

    return 0;
}
int
octree_query_potential_faces(const octree_t* octree, vector_t* result, const wsreal_t aabb[6])
{
    return octree_query_potential_faces_recursive(&octree->root, result, aabb);
}

/* ------------------------------------------------------------------------- */
int
octree_query_point_is_inside_mesh_recursive(const octree_node_t* node, const mesh_t* mesh, const wsreal_t p[3], hashmap_t* tested_indices)
{
    uintptr_t face_idx;
    int intersect_count = 0;
    vec3_t p1, p2;

    /*
     * The way this algorithm works is we project the mesh onto a 2D plane (the
     * easiest way being to just eliminate one of the main axes) and do a
     * line-face intersection test along the eliminated axis through the
     * specified point and count how many faces it intersects. An even number
     * of intersections means the point lies outside of the mesh, and odd
     * number means inside.
     *
     * For this implementation we don't project, we just cast a ray from +Z to
     * -Z through point p, where +Z and -Z lie outside of the octree's bounding
     * box, so the line is guaranteed to intersect all faces in the mesh.
     */

    /* Only select partitions that are leaf nodes */
    if (node->children != NULL)
    {
        int i;
        for (i = 0; i != 8; ++i)
            intersect_count += octree_query_point_is_inside_mesh_recursive(&node->children[i], mesh, p, tested_indices);
        return intersect_count;
    }

    /*
     * We are a leaf node, do intersection test from -Z to +Z for all faces in
     * our partition.
     */
    vec3_copy(&p1, p);
    vec3_copy(&p2, p);
    p1.v.z = AABB_AZ(mesh->aabb) - 1; /* From -Z... */
    p2.v.z = AABB_BZ(mesh->aabb) + 1; /* ...to +Z */
    for (face_idx = 0; face_idx != vector_count(&node->index_buffer)/3; ++face_idx)
    {
        wsret result;
        wsib_t indices[3];
        wsreal_t vertices[9];
        mesh_get_face_indices_from_buffer(indices, node->index_buffer.data, face_idx, mesh->ib_type);

        /* Make sure we don't test duplicates */
        result = hashmap_insert(tested_indices, indices, NULL);
        if (result == WS_KEY_EXISTS)
            continue; /* face was already tested */
        if (result == WS_ERR_OUT_OF_MEMORY)
            return -1;

        /* Get face vertices and do intersection test */
        mesh_get_face_vertices(vertices, mesh, indices);
        intersect_count += intersect_line_triangle_test(p1.xyz, p2.xyz, vertices+0, vertices+3, vertices+6);
    }

    return intersect_count;
}

int
octree_query_point_is_inside_mesh(const octree_t* octree, const wsreal_t p[3])
{
    int result;
    hashmap_t tested_indices;
    hashmap_construct(&tested_indices, sizeof(wsib_t) * 3, 0);
    result = octree_query_point_is_inside_mesh_recursive(&octree->root, octree->mesh, p, &tested_indices);
    hashmap_destruct(&tested_indices);
    return result;
}

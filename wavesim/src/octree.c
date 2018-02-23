#include "wavesim/btree.h"
#include "wavesim/hash.h"
#include "wavesim/intersections.h"
#include "wavesim/log.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/octree.h"
#include "string.h"
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
    /* The root node doesn't own the index buffer like all of the other nodes */
    if (node->parent != NULL)
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
    if (octree->root.children != NULL)
    {
        int i;
        for (i = 0; i != 8; ++i)
            node_destroy_children(&octree->root.children[i]);
        FREE(octree->root.children);
    }

    /* release reference to mesh object */
    octree->mesh = NULL;
    octree->root.index_buffer.data = NULL;
    octree->root.index_buffer.count = 0;
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
#if 0
static void
determine_smallest_subdivision(wsreal_t smallest_subdivision[3], const mesh_t* mesh)
{
    wsib_t f;
    int i;

    /* The smallest face bounding box makes the most sense */
    smallest_subdivision[0] = INFINITY;
    smallest_subdivision[1] = INFINITY;
    smallest_subdivision[2] = INFINITY;
    for (f = 0; f != mesh_face_count(mesh); ++f)
    {
        face_t face = mesh_get_face(mesh, f);
        vec3_t dims = AABB_DIMS(aabb_from_face(&face));
        for (i = 0; i != 3; ++i)
            if (smallest_subdivision[i] > dims.xyz[i] && dims.xyz[i] > WS_EPSILON)
                smallest_subdivision[i] = dims.xyz[i];
    }

    /* If any of the axes are 0 in length, just set it to another axis */
    for (i = 0; i != 3; ++i)
        if (smallest_subdivision[i] <= WS_EPSILON || smallest_subdivision[i] == INFINITY)
            smallest_subdivision[i] = fmax(fmax(smallest_subdivision[0], smallest_subdivision[1]), smallest_subdivision[2]);
}
#endif

/* ------------------------------------------------------------------------- */
static wsret WS_WARN_UNUSED
determine_child_index_buffers(const octree_t* octree, octree_node_t* node)
{
    wsib_t i;
    int c;
    for (i = 0; i != (wsib_t)vector_count(&node->index_buffer); i += 3)
    {
        /*
         * Here we use the index buffer of the *node* (instead of the mesh) to
         * look up 3 vertices in the mesh to form a face. This is dangerous and
         * only works if the octree node's index buffer was initialized with the
         * same datatype as the mesh's index buffer (which, if nothing broke,
         * should always be the case).
         */
        wsib_t indices[3];
        vec3_t face[3];
        indices[0] = mesh_get_index_from_buffer(node->index_buffer.data, i + 0, octree->mesh->ib_type);
        indices[1] = mesh_get_index_from_buffer(node->index_buffer.data, i + 1, octree->mesh->ib_type);
        indices[2] = mesh_get_index_from_buffer(node->index_buffer.data, i + 2, octree->mesh->ib_type);
        face[0] = mesh_get_vertex_position_from_buffer(octree->mesh->vb, indices[0], octree->mesh->vb_type);
        face[1] = mesh_get_vertex_position_from_buffer(octree->mesh->vb, indices[1], octree->mesh->vb_type);
        face[2] = mesh_get_vertex_position_from_buffer(octree->mesh->vb, indices[2], octree->mesh->vb_type);

        /*
         * Test the bounding box of the face we extracted with each child node
         * bounding box. If it intersects, then add the face to the child node.
         */
        aabb_t face_bb = aabb_from_3_points(face[0].xyz, face[1].xyz, face[2].xyz);
        for (c = 0; c != 8; ++c)
        {
            octree_node_t* child = &node->children[c];
            if (intersect_aabb_aabb_test(child->aabb.xyzxyz, face_bb.xyzxyz))
            {
                if (vector_push(&child->index_buffer, &indices[0]) == VECTOR_ERROR) WSRET(WS_ERR_OUT_OF_MEMORY);
                if (vector_push(&child->index_buffer, &indices[1]) == VECTOR_ERROR) WSRET(WS_ERR_OUT_OF_MEMORY);
                if (vector_push(&child->index_buffer, &indices[2]) == VECTOR_ERROR) WSRET(WS_ERR_OUT_OF_MEMORY);
            }
        }
    }

    return WS_OK;
}
static wsret WS_WARN_UNUSED
octree_build_from_mesh_recursive(octree_t* octree, octree_node_t* node, int max_depth)
{
    wsret result;
    unsigned int i;

    /* Stop subdividing when we reach one face */
    if (vector_count(&node->index_buffer) <= 3)
        return WS_OK;

    /* Abort if max depth has been reached */
    if (max_depth == 0)
        return WS_OK;

    /* Subdivide and do AABB intersection tests to fill child index buffers */
    if ((result = octree_subdivide(octree, node)) != WS_OK)
        return result;
    if ((result = determine_child_index_buffers(octree, node)) != WS_OK)
        return result;

    /*
     * If it turns out that all 8 children have the exact same index buffers,
     * then there is no point in doing further subdivisions.
     */
    for (i = 0; i != 8; ++i)
        if (vector_count(&node->index_buffer) != vector_count(&node->children[i].index_buffer))
        {
            for (i = 0; i != 8; ++i)
                if ((result = octree_build_from_mesh_recursive(octree, &node->children[i], max_depth - 1)) != WS_OK)
                    return result;
            return WS_OK;
        }
    /* If we end up here it means the 8 children have identical index buffers.
     * Delete the children, they don't accomplish anything */
    node_destroy_children(node);

    return WS_OK;
}
wsret
octree_build_from_mesh(octree_t* octree, const mesh_t* mesh, int max_depth)
{
    /* Clear old octree if it exists */
    octree_clear(octree);

    /*
     * Create root
     * WARNING: This is a pretty terrible hack; we manually set up the index
     * buffer vector to point to the mesh's index buffer, instead of allocating
     * and copying all of the indices. The root node's IB is identical to the
     * mesh's IB and we save memory by avoiding a copy. Just be extra careful
     * not to delete this vector at any point.
     */
    octree->mesh = mesh;
    octree->root.aabb = mesh->aabb;
    octree->root.index_buffer.capacity = 0;
    octree->root.index_buffer.count = mesh->ib_count;
    octree->root.index_buffer.element_size = mesh->ib_size;
    octree->root.index_buffer.data = mesh->ib;

    /* Handle empty meshes */
    if (mesh_face_count(octree->mesh) == 0)
        return 0;

    if (octree_build_from_mesh_recursive(octree, &octree->root, max_depth) < 0)
        return -1;
    return 0;
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
        size_t node_chunk_idx;
        const vector_t* vn = &node->index_buffer;
        size_t chunk_size = vn->element_size * 3;
        size_t node_chunk_count = vector_count(vn) / 3;
        size_t result_chunk_count = vector_count(result) / 3;

        assert(vector_count(vn) % 3 == 0);
        assert(vector_count(result) % 3 == 0);

        /* For all index triplets in the node... */
        for (node_chunk_idx = 0; node_chunk_idx != node_chunk_count; node_chunk_idx++)
        {
            /* Try to find the current triplet in the result index buffer */
            size_t result_chunk_idx = 0;
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
octree_query_point_is_inside_mesh_recursive(const octree_node_t* node, const mesh_t* mesh, const wsreal_t p[3], btree_t* tested_indices)
{
    wsib_t i;
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
    for (i = 0; i != (wsib_t)vector_count(&node->index_buffer); i += 3)
    {
        wsret result;
        wsib_t indices[3];
        vec3_t vertices[3];
        indices[0] = mesh_get_index_from_buffer(node->index_buffer.data, i + 0, mesh->ib_type);
        indices[1] = mesh_get_index_from_buffer(node->index_buffer.data, i + 1, mesh->ib_type);
        indices[2] = mesh_get_index_from_buffer(node->index_buffer.data, i + 2, mesh->ib_type);

        /* Make sure we don't test duplicates */
        result = btree_insert(tested_indices, hash_face_indices(indices), (void*)1);
        if (result == 1)
            continue; /* face was already tested */
        if (result == -1)
            return -1;

        /* Get face vertices and do intersection test */
        vertices[0] = mesh_get_vertex_position_from_buffer(mesh->vb, indices[0], mesh->vb_type);
        vertices[1] = mesh_get_vertex_position_from_buffer(mesh->vb, indices[1], mesh->vb_type);
        vertices[2] = mesh_get_vertex_position_from_buffer(mesh->vb, indices[2], mesh->vb_type);
        intersect_count += intersect_line_triangle_test(p1.xyz, p2.xyz, vertices[0].xyz, vertices[1].xyz, vertices[2].xyz);
    }

    return intersect_count;
}

int
octree_query_point_is_inside_mesh(const octree_t* octree, const wsreal_t p[3])
{
    int result;
    btree_t tested_indices;
    btree_construct(&tested_indices);
    result = octree_query_point_is_inside_mesh_recursive(&octree->root, octree->mesh, p, &tested_indices);
    btree_clear_free(&tested_indices);
    return result;
}

#include "wavesim/octree.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/intersections.h"
#include "string.h"

#define IDX(x, y, z) \
    x*4 + y*2 + z

#define CX(idx) \
    (idx >= 4 ? 1 : 0)

#define CY(idx) \
    ((idx >= 2 && idx < 4) || idx >= 6 ? 1 : 0)

#define CZ(idx) \
    (idx % 2 ? 1 : 0)

/* ------------------------------------------------------------------------- */
octree_t*
octree_create(void)
{
    octree_t* octree = (octree_t*)MALLOC(sizeof *octree);
    if (octree == NULL)
        OUT_OF_MEMORY(NULL);
    octree_construct(octree);
    return octree;
}

/* ------------------------------------------------------------------------- */
void
octree_construct(octree_t* octree)
{
    memset(octree, 0, sizeof *octree);
    vector_construct(&octree->index_buffer, sizeof(int));
}

/* ------------------------------------------------------------------------- */
void
octree_destruct(octree_t* octree)
{
    octree_clear(octree);
    vector_clear_free(&octree->index_buffer);
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
octree_clear(octree_t* octree)
{
    int i;
    if (octree->children == NULL)
        return;

    /* Destroy all children recursively */
    for (i = 0; i != 8; ++i)
    {
        octree_destruct(&octree->children[i]);
    }
    FREE(octree->children);
    octree->children = NULL;

    /* Clear list of faces */
    vector_clear_free(&octree->index_buffer);

    /* release reference to mesh object */
    octree->mesh = NULL;
}

/* ------------------------------------------------------------------------- */
int
octree_subdivide(octree_t* octree)
{
    int i;
    real width, height, depth;

    /* Subdivide is only valid for leaf nodes */
    if (octree->children != NULL)
        return 1;

    /* Create 8 new children */
    octree->children = (octree_t*)MALLOC(sizeof(octree_t) * 8);
    if (octree->children == NULL)
        OUT_OF_MEMORY(-1);

    /* Calculate child bounding boxes */
    width  = (octree->aabb.b.v.x - octree->aabb.a.v.x) * 0.5;
    height = (octree->aabb.b.v.y - octree->aabb.a.v.y) * 0.5;
    depth  = (octree->aabb.b.v.z - octree->aabb.a.v.z) * 0.5;
    for (i = 0; i != 8; ++i)
    {
        octree_construct(&octree->children[i]);
        octree->children[i].parent = octree;

        octree->children[i].aabb.a.v.x = octree->aabb.a.v.x + (CX(i) + 0) * width;
        octree->children[i].aabb.b.v.x = octree->aabb.a.v.x + (CX(i) + 1) * width;
        octree->children[i].aabb.a.v.y = octree->aabb.a.v.y + (CY(i) + 0) * height;
        octree->children[i].aabb.b.v.y = octree->aabb.a.v.y + (CY(i) + 1) * height;
        octree->children[i].aabb.a.v.z = octree->aabb.a.v.z + (CZ(i) + 0) * depth;
        octree->children[i].aabb.b.v.z = octree->aabb.a.v.z + (CZ(i) + 1) * depth;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
static intptr_t
octree_build_from_mesh_recursive(octree_t* octree, const mesh_t* mesh, vec3_t smallest_subdivision)
{
    int i;
    int total_faces;

    (void)octree;
    (void)mesh;
    (void)smallest_subdivision;
    (void)i;
    (void)total_faces;

    return 0;

    /*
     * NOTE: We are copying *pointers* to faces into the octree's vector, not
     * the face object itself. This is to save memory, since vertex_t has quite
     * a few fields and the octree is going to hold many lists containing the
     * same faces multiple times.
     *
    VECTOR_FOR_EACH(&octree->parent->faces, face_t*, ppface)
        if (intersect_aabb_face_test(octree->aabb, *ppface) == 1)
            if (vector_push(&octree->faces, ppface) < 0)
                return -1;
    VECTOR_END_EACH
    total_faces = octree_face_count(octree);

    * Stop subdividing when we reach one face *
    if (vector_count(&octree->faces) <= 1)
        return total_faces;

    * Abort if below smallest division *
    for (i = 0; i != 3; ++i)
        if (octree->aabb.b.f[i] - octree->aabb.a.f[i] < smallest_subdivision.f[i])
            return total_faces;

    if (octree_subdivide(octree) < 0)
        return -1;

    for (int i = 0; i != 8; ++i)
        total_faces += octree_build_from_mesh_recursive(&octree->children[i], mesh, smallest_subdivision);
    return total_faces;*/
}
intptr_t
octree_build_from_mesh(octree_t* octree, const mesh_t* mesh, vec3_t smallest_subdivision)
{
    int i, total_faces;

    (void)i;
    (void)total_faces;
    return octree_build_from_mesh_recursive(octree, mesh, smallest_subdivision);

    /* Clear old octree if it exists *
    octree_clear(octree);

    * Copy all faces into top-level octree node *
    octree->aabb = mesh->aabb;
    VECTOR_FOR_EACH(&mesh->index_buffer, face_t, face)
        if (vector_push(&octree->index_buffer, &face) < 0)
            return -1;
    VECTOR_END_EACH

    * Recursively subdivide *
    if (octree_subdivide(octree) < 0)
        return -1;
    total_faces = 0;
    for (i = 0; i != 8; ++i)
    {
        int face_count;
        if ((face_count = octree_build_from_mesh_recursive(&octree->children[i], mesh, smallest_subdivision)) < 0)
            return -1;
        total_faces += face_count;
    }

    return total_faces + octree_face_count(octree);*/
}

/* ------------------------------------------------------------------------- */
const vector_t*
octree_intersect_aabb(const octree_t* octree, aabb_t aabb)
{
    (void)octree;
    (void)aabb;
    return NULL;
}

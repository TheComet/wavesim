#include "wavesim/partition.h"
#include "wavesim/memory.h"
#include "wavesim/octree.h"

typedef struct partition_area_t
{
    aabb_t aabb;
    WS_REAL sound_speed;
} partition_area_t;

/* ------------------------------------------------------------------------- */
partition_t*
partition_create(void)
{
    partition_t* md = (partition_t*)MALLOC(sizeof *md);
    if (md == NULL)
        OUT_OF_MEMORY(NULL);
    partition_construct(md);
    return md;
}

/* ------------------------------------------------------------------------- */
void
partition_destroy(partition_t* partition)
{
    vector_clear_free(&partition->areas);
    FREE(partition);
}

/* ------------------------------------------------------------------------- */
void
partition_construct(partition_t* partition)
{
    vector_construct(&partition->areas, sizeof(partition_area_t));
}

/* ------------------------------------------------------------------------- */
void
partition_destruct(partition_t* partition)
{
    vector_clear_free(&partition->areas);
}

/* ------------------------------------------------------------------------- */
int
partition_add_area(partition_t* partition, aabb_t bounding_box, WS_REAL sound_speed)
{
    partition_area_t* area = vector_emplace(&partition->areas);
    if (area == NULL)
        OUT_OF_MEMORY(-1);

    area->aabb = bounding_box;
    area->sound_speed = sound_speed;

    return 0;
}

/* ------------------------------------------------------------------------- */
int
partition_build_from_mesh(partition_t* partition,
                       const partition_t* medium,
                       const mesh_t* mesh,
                       vec3_t grid_size)
{
    octree_t octree;
    octree_construct(&octree);
    if (octree_build_from_mesh(&octree, mesh, grid_size) < 0)
        return -1;

    (void)partition;
    (void)medium;

    return 0;
}

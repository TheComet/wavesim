#include "wavesim/partition.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/octree.h"

typedef struct partition_area_t
{
    aabb_t aabb;
    WS_REAL sound_speed;
    vector_t adcacent_areas; /* partition_area_t */
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
    partition_destruct(partition);
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
    partition_clear(partition);
}

/* ------------------------------------------------------------------------- */
void
partition_clear(partition_t* partition)
{
    VECTOR_FOR_EACH(&partition->areas, partition_area_t, area)
        vector_clear_free(&area->adcacent_areas);
    VECTOR_END_EACH
    vector_clear_free(&partition->areas);
}

/* ------------------------------------------------------------------------- */
int
partition_add_area(partition_t* partition, const WS_REAL bb[6], WS_REAL sound_speed)
{
    partition_area_t* area = vector_emplace(&partition->areas);
    if (area == NULL)
        OUT_OF_MEMORY(-1);

    area->aabb = aabb(bb[0], bb[1], bb[2], bb[3], bb[4], bb[5]);
    area->sound_speed = sound_speed;
    vector_construct(&area->adcacent_areas, sizeof(partition_area_t));

    return 0;
}

/* ------------------------------------------------------------------------- */
static int
do_partition(partition_t* partition,
             const octree_t* octree)
{
    (void)partition;
    (void)octree;
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
    if (octree_build_from_mesh(&octree, mesh, partition->grid_size) < 0)
        return -1;

    partition->grid_size = grid_size;
    partition->boundary = medium == NULL ? mesh->aabb : medium->boundary;
    do_partition(partition, &octree);

    (void)medium;

    octree_destruct(&octree);
    return 0;
}

#include "wavesim/attribute.h"
#include "wavesim/intersections.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/octree.h"
#include "wavesim/partition.h"
#include <string.h>

typedef struct partition_area_t
{
    aabb_t aabb;
    WS_REAL sound_speed;
    vector_t adcacent_areas; /* int32_t (indices into partition->areas) */
} partition_area_t;

static int
determine_cell_attribute(attribute_t* cell_attribute,
                         const octree_t* octree,
                         const WS_REAL cell_aabb[6])
{
    unsigned int i;
    vector_t query_result;

    /* XXX This is super ugly, maybe add a result_construct() function that takes a mesh? */
    vector_construct(&query_result, octree->mesh->ib_size);
    if (octree_query_aabb(octree, &query_result, cell_aabb) != 0)
        goto octree_query_failed;

    /*
     * Octree delivers a number of faces that *might* intersect the cell AABB,
     * but we cannot be sure until we do a proper intersection test.
     */
    {
        vec3_t rta_avg;  /* absorption, reflection, transmission average of all
                            intersecting faces */
        vec3_set_zero(rta_avg.xyz);

        for (i = 0; i != vector_count(&query_result) / 3; ++i)
        {

            /* Do intersection test of face with our cell */
            intersect_result_t intersect_result ;
            const mesh_t* m = octree->mesh;
            face_t face = mesh_get_face_from_buffers(m->vb, query_result.data, m->ab,
                                                    i, m->vb_type, m->ib_type);
            if (intersect_face_aabb(&intersect_result, &face, cell_aabb) == 0)
                continue; /* face doesn't intersect our cell, so ignore it */

            /*
             * Average the intersection positions, they all lie in the face's
             * plane and will allow us to interpolate the vertex attributes
             * accordingly.
             */
            {
                int j;
                vec3_t avg_pos;
                attribute_t attr;

                vec3_set_zero(avg_pos.xyz);
                for (j = 0; j != intersect_result.count; ++j)
                    vec3_add_vec3(avg_pos.xyz, intersect_result.shape[j].xyz);
                vec3_div_scalar(avg_pos.xyz, intersect_result.count);

                face_interpolate_attributes_barycentric(&face, &attr, avg_pos.xyz);
                rta_avg.v.x += attr.absorption;
                rta_avg.v.y += attr.reflection;
                rta_avg.v.z += attr.transmission;
            }
        }

        /* It's possible that no faces intersected */
        if (vec3_is_zero(rta_avg.xyz))
        {
            attribute_set_default(cell_attribute);
        }
        else
        {
            /*
             * Average and copy the "rta" vector, which is the sum of all
             * attributes, into our return value. that
             * 1 = reflection + transmission + absorption.
             */
            vec3_div_scalar(rta_avg.xyz, vector_count(&query_result));
            cell_attribute->absorption = rta_avg.v.x;
            cell_attribute->reflection = rta_avg.v.y;
            cell_attribute->transmission = rta_avg.v.z;
        }
    }

    octree_query_failed : vector_clear_free(&query_result);
    return -1;
}

/* ------------------------------------------------------------------------- */
wsret
partition_create(partition_t** partition)
{
    *partition = MALLOC(sizeof **partition);
    if (*partition == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    partition_construct(*partition);
    return WS_OK;
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
    partition->decompose = partition_decompose_systematic;
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
wsret
partition_add_area(partition_t* partition, const WS_REAL bb[6], WS_REAL sound_speed)
{
    partition_area_t* area = vector_emplace(&partition->areas);
    if (area == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    area->aabb = aabb(bb[0], bb[1], bb[2], bb[3], bb[4], bb[5]);
    area->sound_speed = sound_speed;
    vector_construct(&area->adcacent_areas, sizeof(int32_t));

    return 0;
}

/* ------------------------------------------------------------------------- */
void
partition_set_decomposition_method(partition_t* partition,
                                   partition_decomposition_func method)
{
    partition->decompose = method;
}

/* ------------------------------------------------------------------------- */
typedef enum direction_e
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    DIRECTION_COUNT
} direction_e;
static aabb_t
get_adjacent_slice(const partition_t* partition, const WS_REAL aabb[6], direction_e direction)
{
    aabb_t adjacent;
    memcpy(adjacent.xyzxyz, aabb, sizeof(adjacent.xyzxyz));
    switch (direction)
    {
        case UP :
            AABB_AY(adjacent) = aabb[4];
            AABB_BY(adjacent) = aabb[4] + partition->grid_size.v.y;
            break;
        case DOWN:
            AABB_AY(adjacent) = aabb[1] - partition->grid_size.v.y;
            AABB_BY(adjacent) = aabb[1];
            break;
        case LEFT :
            AABB_AX(adjacent) = aabb[0] - partition->grid_size.v.x;
            AABB_BX(adjacent) = aabb[0];
            break;
        case RIGHT:
            AABB_AX(adjacent) = aabb[3];
            AABB_BX(adjacent) = aabb[3] + partition->grid_size.v.x;
            break;
        case FRONT :
            AABB_AZ(adjacent) = aabb[2] - partition->grid_size.v.z;
            AABB_BZ(adjacent) = aabb[2];
            break;
        case BACK:
            AABB_AZ(adjacent) = aabb[5];
            AABB_BZ(adjacent) = aabb[5] + partition->grid_size.v.z;
            break;
        default: break;
    }

    return adjacent;
}
static partition_area_t*
partition_area_already_occupied(const partition_t* partition, const WS_REAL aabb[6])
{
    VECTOR_FOR_EACH(&partition->areas, partition_area_t, area)
        if (intersect_aabb_aabb_test(area->aabb.xyzxyz, aabb))
            return area;
    VECTOR_END_EACH

    return NULL;
}
static wsret
decompose_systematic_recursive(partition_t* partition,
                               int32_t parent_area_idx,
                               const octree_t* octree,
                               const partition_t* medium,
                               aabb_t seed)
{
    int direction;
    int occupied_slices;
    vector_t potential_new_seeds;
    int32_t this_area_idx;

    /* Determine the cell type of our seed */
    attribute_t seed_attr;
    determine_cell_attribute(&seed_attr, octree, seed.xyzxyz);

    /*
     * Try to expand the seed evenly in all directions, until we hit an adjacent
     * cell that has different attributes.
     */
    vector_construct(&potential_new_seeds, sizeof(aabb_t));
    occupied_slices = 0;
    while (occupied_slices < DIRECTION_COUNT)
    {
        for (direction = 0; direction != DIRECTION_COUNT; ++direction)
        {
            aabb_t slice;
            aabb_t cell;

            /* Calculate a slice adjacent to this seed and make sure it doesn't
             * already exist in the partition. */
            slice = get_adjacent_slice(partition, seed.xyzxyz, direction);
            if (partition_area_already_occupied(partition, slice.xyzxyz) != NULL)
            {
                ++occupied_slices;
                continue;
            }

            /* Iterate through all cells in the slice and confirm that these cells
             * have the same attributes as our seed cell */
            cell = aabb(
                AABB_AX(slice), AABB_AY(slice), AABB_AZ(slice),
                AABB_AX(slice) + partition->grid_size.v.x,
                AABB_AY(slice) + partition->grid_size.v.y,
                AABB_AZ(slice) + partition->grid_size.v.z
            );
            while (AABB_BX(cell) <= AABB_BX(slice))
            {
                while (AABB_BY(cell) <= AABB_BY(slice))
                {
                    while (AABB_BZ(cell) <= AABB_BZ(slice))
                    {
                        attribute_t cell_attribute;
                        determine_cell_attribute(&cell_attribute, octree, cell.xyzxyz);
                        if (attribute_is_same(&seed_attr, &cell_attribute) != 0)
                        {
                            aabb_t* new_seed = vector_emplace(&potential_new_seeds);
                            if (new_seed == NULL)
                                WSRET(WS_ERR_OUT_OF_MEMORY);
                            *new_seed = cell;
                        }
                        AABB_AZ(cell) += partition->grid_size.v.z;
                        AABB_BZ(cell) += partition->grid_size.v.z;
                    }
                    AABB_AZ(cell) = AABB_AZ(slice); /* Reset Z coordinates for next ieration */
                    AABB_BZ(cell) = AABB_AZ(slice) + partition->grid_size.v.z;
                    AABB_AY(cell) += partition->grid_size.v.y;
                    AABB_BY(cell) += partition->grid_size.v.y;
                }
                AABB_AY(cell) = AABB_AY(slice); /* Reset Y coordinates for next iteration */
                AABB_BY(cell) = AABB_AY(slice) + partition->grid_size.v.y;
                AABB_AX(cell) += partition->grid_size.v.x;
                AABB_BX(cell) += partition->grid_size.v.x;
            }

            /* Since slice has the same attributes, we can merge it with our
             * seed now */
            aabb_expand_aabb(seed.xyzxyz, slice.xyzxyz);
        }
    }

    /*
     * At this point, the seed has been expanded as much as possible. Add it to
     * the partition as a new area.
     */
    this_area_idx = vector_count(&partition->areas);
    if (partition_add_area(partition, seed.xyzxyz, 1) != 0)
        return -1;

    /* Add ourselves to the parent area's adjacent list, if possible */
    if (parent_area_idx > -1)
    {
        partition_area_t* parent_area = vector_get_element(&partition->areas, parent_area_idx);
        int32_t* adjacent_area_idx = vector_emplace(&parent_area->adcacent_areas);
        if (adjacent_area_idx == NULL)
            WSRET(WS_ERR_OUT_OF_MEMORY);
        *adjacent_area_idx = this_area_idx;
    }

    /*
     * During expansion, we tracked which cells had different attributes than
     * our own. All of these cells are potential new seeds from which we can
     * expand new areas. Do this now.
     */
    VECTOR_FOR_EACH(&potential_new_seeds, aabb_t, new_seed)
        wsret result;
        if (partition_area_already_occupied(partition, new_seed->xyzxyz) != NULL)
            continue;
        result = decompose_systematic_recursive(partition, this_area_idx, octree, medium, *new_seed);
        if (result != WS_OK)
            return result;
    VECTOR_END_EACH

    (void)medium;
    return WS_OK;
}
wsret
partition_decompose_systematic(partition_t* partition,
                               const octree_t* octree,
                               const partition_t* medium)
{
    /* Start at the bottom, left, front corner */
    aabb_t seed = aabb(
        AABB_AX(partition->boundary),
        AABB_AY(partition->boundary),
        AABB_AZ(partition->boundary),
        AABB_AX(partition->boundary) + partition->grid_size.v.x,
        AABB_AY(partition->boundary) + partition->grid_size.v.y,
        AABB_AZ(partition->boundary) + partition->grid_size.v.z
    );
    return decompose_systematic_recursive(partition, -1, octree, medium, seed);
}

/* ------------------------------------------------------------------------- */
int
partition_decompose_greedy_random(partition_t* partition,
                                  const octree_t* octree,
                                  const partition_t* medium)
{
    (void)partition;
    (void)octree;
    (void)medium;
    return 0;
}

/* ------------------------------------------------------------------------- */
int
partition_build_from_mesh(partition_t* partition,
                          const partition_t* medium,
                          const mesh_t* mesh,
                          const WS_REAL grid_size[3])
{
    octree_t octree;

    /* Clear areas from last time */
    vector_clear_free(&partition->areas);

    /* Copy arguments into partition structure, partition building relies on
     * them */
    vec3_copy(&partition->grid_size, grid_size);
    partition->boundary = medium == NULL ? mesh->aabb : medium->boundary;

    /* Need an octree */
    octree_construct(&octree);
    if (octree_build_from_mesh(&octree, mesh, partition->grid_size) < 0)
        return -1;

    partition->decompose(partition, &octree, medium);

    octree_destruct(&octree);
    return 0;
}

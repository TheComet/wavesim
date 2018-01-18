#include "wavesim/attribute.h"
#include "wavesim/intersections.h"
#include "wavesim/memory.h"
#include "wavesim/mesh.h"
#include "wavesim/octree.h"
#include "wavesim/partition.h"

typedef struct partition_area_t
{
    aabb_t aabb;
    WS_REAL sound_speed;
    vector_t adcacent_areas; /* partition_area_t */
} partition_area_t;

static int
determine_cell_attribute(attribute_t* cell_attribute,
                         const octree_t* octree,
                         const WS_REAL cell_aabb[6])
{
    unsigned int i;
    vector_t query_result;

    vector_construct(&query_result, octree->mesh->ib_size);
    if (octree_query_aabb(octree, &query_result, cell_aabb) != 0)
        goto octree_query_failed;

    /*
     * Octree delivers a number of faces that *might* intersect the cell AABB,
     * but we cannot be sure until we do a proper intersection test.
     */
    {
        vec3_t art_avg;  /* absorption, reflection, transmission average of all
                            intersecting faces */
        vec3_set_zero(art_avg.xyz);

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
                art_avg.v.x += attr.absorption;
                art_avg.v.y += attr.reflection;
                art_avg.v.z += attr.transmission;
            }
        }

        /* It's possible that no faces intersected */
        if (vec3_is_zero(art_avg.xyz))
        {
            attribute_set_default(cell_attribute);
        }
        else
        {
            /*
             * Copy the "art" vector, which is the sum of all attributes, into our
             * return value. Note that 1 = absorption + reflection + transmission,
             * which will be the case if we normalize the "art" vector.
             */
            vec3_normalize(art_avg.xyz);
            cell_attribute->absorption = art_avg.v.x;
            cell_attribute->reflection = art_avg.v.y;
            cell_attribute->transmission = art_avg.v.z;
        }
    }

    octree_query_failed : vector_clear_free(&query_result);
    return -1;
}

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
void
partition_set_decomposition_method(partition_t* partition,
                                   partition_decomposition_func method)
{
    partition->decompose = method;
}

/* ------------------------------------------------------------------------- */
static int
decompose_systematic_recursive(partition_t* partition,
                               const octree_t* octree,
                               const partition_t* medium,
                               aabb_t seed)
{
    /* Determine the cell type of our seed */
    attribute_t seed_attr;
    determine_cell_attribute(&seed_attr, octree, seed.xyzxyz);

    (void)partition;
    (void)medium;
    return 0;
}
int
partition_decompose_systematic(partition_t* partition,
                               const octree_t* octree,
                               const partition_t* medium)
{
    return decompose_systematic_recursive(partition, octree, medium, aabb_reset());
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

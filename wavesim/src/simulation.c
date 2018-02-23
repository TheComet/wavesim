#include "wavesim/simulation.h"
#include "wavesim/memory.h"
#include "wavesim/vector.h"
#include "wavesim/vec3.h"
#include <math.h>
#include <assert.h>
#include <fftw3.h>

#define MODES_GET(buffer) \
    buffer

typedef struct partition_state_t
{
    fftw_plan dct_plan;
    fftw_plan idct_plan;
    wsreal_t* modes;
    int dims[3];
} partition_state_t;

typedef struct simulation_state_t
{
    vector_t partitions[3]; /* partition_state_t */
    int time_step_mode_idx;
    wsreal_t time;
} simulation_state_t;

/* ------------------------------------------------------------------------- */
wsret
simulation_create(simulation_t** simulation, const medium_t* medium)
{
    *simulation = MALLOC(sizeof **simulation);
    if (*simulation == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    simulation_construct(*simulation, medium);
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
simulation_destroy(simulation_t* simulation)
{
    simulation_destruct(simulation);
    FREE(simulation);
}

/* ------------------------------------------------------------------------- */
void
simulation_construct(simulation_t* simulation, const medium_t* medium)
{
    simulation->medium = medium;
}

/* ------------------------------------------------------------------------- */
void
simulation_destruct(simulation_t* simulation)
{
    (void)simulation;
}

/* ------------------------------------------------------------------------- */
wsret
simulation_prepare(simulation_t* simulation)
{
    size_t i;
    int t;
    simulation_state_t* state;
    size_t partition_count = medium_partition_count(simulation->medium);

    assert(simulation->medium != NULL);

    state = simulation->state = MALLOC(sizeof(simulation_state_t));
    if (simulation->state == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    /*
     * Allocate a 3-dimensional array that can hold all modes (x, y, z) over
     * 3 time steps.
     */
    for (t = 0; t != 3; ++t)
    {
        vector_construct(&state->partitions[t], sizeof(partition_state_t));
        vector_resize(&state->partitions[t], partition_count);
        for (i = 0; i != partition_count; ++i)
        {
            medium_partition_t* partition = vector_get_element(&simulation->medium->partitions, i);
            partition_state_t* partition_state = vector_get_element(&state->partitions[t], i);

            /* calculate the number of cells in each dimension and allocate flat array */
            fftw_iodim dims[3] = {{0, 1, 1}, {0, 1, 1}, {0, 1, 1}};
            vec3_t cell_count = AABB_DIMS(partition->aabb);
            vec3_div_scalar(cell_count.xyz, simulation->cell_size);
            dims[0].n = partition_state->dims[0] = (int)ceil(cell_count.v.x);
            dims[1].n = partition_state->dims[1] = (int)ceil(cell_count.v.y);
            dims[2].n = partition_state->dims[2] = (int)ceil(cell_count.v.z);
            partition_state->modes = fftw_malloc((size_t)(dims[0].n * dims[1].n * dims[2].n));

            /* Create plans */
            {
                fftw_iodim howmany_dims[1] = {{1, 1, 1}};
                fftw_r2r_kind dct_kind[1] = {FFTW_RODFT10};
                fftw_r2r_kind idct_kind[1] = {FFTW_RODFT01};
                partition_state->dct_plan = fftw_plan_guru_r2r(
                    3, dims,
                    1, howmany_dims,
                    partition_state->modes, partition_state->modes,
                    dct_kind, FFTW_MEASURE);
                partition_state->idct_plan = fftw_plan_guru_r2r(
                    3, dims,
                    1, howmany_dims,
                    partition_state->modes, partition_state->modes,
                    idct_kind, FFTW_MEASURE);
            }

            /* Initialize all modes to 0 */
            {
                int x;
                for (x = 0; x != dims[0].n * dims[1].n * dims[2].n; ++x)
                    partition_state->modes[x] = 0.0;
            }
        }
    }

    /* Initialize a few other things */
    state->time_step_mode_idx = 0;
    state->time = 0.0;

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
simulation_finalize(simulation_t* simulation)
{
    size_t i;
    int t;
    simulation_state_t* state = simulation->state;

    assert(simulation->medium != NULL);

    for (t = 0; t != 3; ++t)
    {
        for (i = 0; i != vector_count(&state->partitions[t]); ++i)
        {
            partition_state_t* partition_state = vector_get_element(&state->partitions[t], i);
            fftw_destroy_plan(partition_state->dct_plan);
            fftw_destroy_plan(partition_state->idct_plan);
            fftw_free(partition_state->modes);
        }
        vector_clear_free(&state->partitions[t]);
    }

    FREE(state);
    simulation->state = NULL;
}

/* ------------------------------------------------------------------------- */
void
simulation_advance(simulation_state_t* state, wsreal_t dt)
{
    size_t part_idx;
    int idx[3];
    int i;

    idx[0] = (state->time_step_mode_idx - 1) % 3;
    idx[1]= state->time_step_mode_idx;
    idx[2] = (state->time_step_mode_idx + 1) % 3;
    state->time_step_mode_idx = idx[2];

    for (part_idx = 0; part_idx != vector_count(&state->partitions[0]); ++part_idx)
    {
        partition_state_t* prev_state = vector_get_element(&state->partitions[idx[0]], part_idx);
        partition_state_t* curr_state = vector_get_element(&state->partitions[idx[1]], part_idx);
        partition_state_t* next_state = vector_get_element(&state->partitions[idx[2]], part_idx);

        for (i = 0; i != next_state->dims[0]*next_state->dims[1]*next_state->dims[2]; ++i)
        {
            next_state->modes[i] = 2*curr_state->modes[i]*cos(dt) - prev_state->modes[i] + /*TODO*/(1.0 - cos(dt));
        }
    }
}

/* ------------------------------------------------------------------------- */
wsret
simulation_execute(simulation_t* simulation)
{
    simulation_advance(simulation->state);
    return WS_OK;
}

#include "wavesim/simulation/simulation_ard.h"
#include <math.h>
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
    partition_state_t* partition_states[3];  /* We store the states over 3 time steps */
    int time_step_mode_idx;
    wsreal_t time;
    wsreal_t* modes_buffer;
    partition_state_t* states_buffer;
} simulation_state_t;

/* ------------------------------------------------------------------------- */
wsret
simulation_ard_prepare(simulation_t* simulation)
{
    (void)simulation;
    /*
     * FFTW results are different than MATLAB's:
     *   https://www.dsprelated.com/showthread/comp.dsp/93988-1.php
     *

    simulation_state_t* state;
    fftw_iodim dims;
    uintptr_t i, t;
    uintptr_t partition_count;
    uintptr_t total_cell_count;
    uintptr_t cell_memory_required;
    uintptr_t partition_memory_required;
    wsreal_t* modes_buffer_ptr;

    if (simulation->medium == NULL)
        WSRET(WS_ERR_SIM_MEDIUM_NOT_SET);
    if (simulation->audio_source == NULL)
        WSRET(WS_ERR_SIM_AUDIO_SOURCE_NOT_SET);
    if (simulation->audio_listener == NULL)
        WSRET(WS_ERR_SIM_AUDIO_LISTENER_NOT_SET);

    state = simulation->state = MALLOC(sizeof(simulation_state_t));
    if (simulation->state == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    * Update components with simulation's resolution settings *
    medium_set_resolution(simulation->medium, simulation->max_frequency, simulation->cell_tolerance);
    * TODO set audio listener/player max frequency here too *

    *
     * To avoid having to allocate memory millions of times, count the total
     * amount of cells in the scene, count the total number of partitions in
     * the scene, and then allocate two buffers and divide the memory up
     * appropriately.
     *
     * Note that it's not possible to simply look at the bounding box of the
     * medium and determine the total cell count from that. The medium is not
     * guaranteed to be rectangular in shape. We have to count the cells in
     * each partition individually.
     *
    partition_count = medium_partition_count(simulation->medium);
    total_cell_count = medium_cell_count(simulation->medium);
    partition_memory_required = partition_count * 3 * sizeof(partition_state_t);
    cell_memory_required = total_cell_count * 3 * sizeof(wsreal_t);
    ws_log_info(&g_ws_log, "[SIM] There are %d partitions, %d cells. Total memory requirement is %.2f GiB",
                partition_count, total_cell_count,
                (wsreal_t)(partition_memory_required + cell_memory_required) / (1024*1024*1024));
    state->modes_buffer = fftw_malloc(cell_memory_required);
    state->states_buffer = MALLOC(partition_memory_required);

    *
     * Allocate a 3-dimensional array that can hold all modes (x, y, z) over
     * 3 time steps.
     *
    modes_buffer_ptr = state->modes_buffer;
    for (t = 0; t != 3; ++t)
    {
        for (i = 0; i != partition_count; ++i)
        {
            medium_partition_t* partition = medium_get_partition(simulation->medium, i);
            partition_state_t* partition_state = state->states_buffer + t*partition_count + i;

            partition_state->modes = modes_buffer_ptr;
            modes_buffer_ptr += partition->cell_count[0]*partition->cell_count[1]*partition->cell_count[2];
        }
    }

    *
     * Initialize all modes to 0 *after* having created the fftw plans, because
     * fftw potentially modifies values in the buffer during plan creation
     *
    for (i = 0; i != cell_memory_required; ++i)
        state->modes_buffer[i] = 0.0;

    * Initialize a few other things *
    state->time_step_mode_idx = 0;
    state->time = 0.0;
*/
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
simulation_ard_finalize(simulation_t* simulation)
{
    (void)simulation;
    /*
    uintptr_t i;
    int t;
    simulation_state_t* state = simulation->state;

    assert(simulation->medium != NULL);

    for (t = 0; t != 3; ++t)
    {
        for (i = 0; i != vector_count(&state->partition_states[t]); ++i)
        {
            partition_state_t* partition_state = vector_get_element(&state->partition_states[t], i);
            fftw_destroy_plan(partition_state->dct_plan);
            fftw_destroy_plan(partition_state->idct_plan);
            fftw_free(partition_state->modes);
        }
        vector_clear_free(&state->partition_states[t]);
    }

    FREE(state);
    simulation->state = NULL;*/
}

/* ------------------------------------------------------------------------- */
int
simulation_ard_advance(simulation_t* simulation, wsreal_t dt)
{
    (void)simulation;
    (void)dt;
    /*
    uintptr_t part_idx;
    int idx[3];
    int i;
    simulation_state_t* state = simulation->state;

    idx[0] = (state->time_step_mode_idx - 1) % 3;
    idx[1]= state->time_step_mode_idx;
    idx[2] = (state->time_step_mode_idx + 1) % 3;
    state->time_step_mode_idx = idx[2];

    for (part_idx = 0; part_idx != vector_count(&state->partition_states[0]); ++part_idx)
    {
        partition_state_t* prev_state = vector_get_element(&state->partition_states[idx[0]], part_idx);
        partition_state_t* curr_state = vector_get_element(&state->partition_states[idx[1]], part_idx);
        partition_state_t* next_state = vector_get_element(&state->partition_states[idx[2]], part_idx);

        for (i = 0; i != next_state->dims[0]*next_state->dims[1]*next_state->dims[2]; ++i)
        {
            next_state->modes[i] = 2*curr_state->modes[i]*cos(simulation->dt) - prev_state->modes[i] + (1.0 - cos(simulation->dt));
        }
    }*/
    return 0;
}

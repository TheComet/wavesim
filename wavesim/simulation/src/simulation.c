#include "wavesim/memory.h"
#include "wavesim/vector.h"
#include "wavesim/vec3.h"
#include "wavesim/simulation/audio_source.h"
#include "wavesim/simulation/simulation.h"
#include "wavesim/simulation/simulation_ard.h"
#include "wavesim/simulation/simulation_ray.h"
#include <assert.h>
#include <stddef.h>

/* ------------------------------------------------------------------------- */
wsret
simulation_create(simulation_t** simulation, simulation_type_e type)
{
    *simulation = MALLOC(sizeof **simulation);
    if (*simulation == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    simulation_construct(*simulation, type);
    WSRET(WS_OK);
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
simulation_construct(simulation_t* simulation, simulation_type_e type)
{
    simulation_set_type(simulation, type);
}

/* ------------------------------------------------------------------------- */
void
simulation_destruct(simulation_t* simulation)
{
    (void)simulation;
}

/* ------------------------------------------------------------------------- */
void
simulation_set_type(simulation_t* simulation, enum simulation_type_e type)
{
    switch (type)
    {
        case WAVESIM_ARD:
            simulation->prepare  = simulation_ard_prepare;
            simulation->advance  = simulation_ard_advance;
            simulation->finalize = simulation_ard_finalize;
            break;

        case WAVESIM_RAY:
            simulation->prepare  = simulation_ray_prepare;
            simulation->advance  = simulation_ray_advance;
            simulation->finalize = simulation_ray_finalize;
            break;
    }
}

/* ------------------------------------------------------------------------- */
void
simulation_set_resolution(simulation_t* simulation, wsreal_t max_frequency, wsreal_t cell_tolerance)
{
    simulation->max_frequency = max_frequency;
    simulation->cell_tolerance = cell_tolerance;
}

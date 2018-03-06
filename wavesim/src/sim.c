#include "wavesim/audio_source.h"
#include "wavesim/memory.h"
#include "wavesim/simulation.h"
#include "wavesim/simulation_ard.h"
#include "wavesim/simulation_ray.h"
#include "wavesim/vector.h"
#include "wavesim/vec3.h"
#include <assert.h>

/* ------------------------------------------------------------------------- */
wsret
simulation_create(simulation_t** simulation, simulation_type_e type)
{
    *simulation = MALLOC(sizeof **simulation);
    if (*simulation == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    simulation_construct(*simulation, type);
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
        case WS_ARD:
            simulation->prepare  = simulation_ard_prepare;
            simulation->advance  = simulation_ard_advance;
            simulation->finalize = simulation_ard_finalize;
            break;

        case WS_RAY:
            simulation->prepare  = simulation_ray_prepare;
            simulation->advance  = simulation_ray_advance;
            simulation->finalize = simulation_ray_finalize;
            break;
    }
}

/* ------------------------------------------------------------------------- */
WAVESIM_PUBLIC_API void
simulation_set_audio_source(simulation_t* simulation, audio_source_t* as)
{
    simulation->audio_source = as;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PUBLIC_API void
simulation_set_audio_listener(simulation_t* simulation, audio_listener_t* al)
{
    simulation->audio_listener = al;
}

/* ------------------------------------------------------------------------- */
void
simulation_set_resolution(simulation_t* simulation, wsreal_t max_frequency, wsreal_t cell_tolerance)
{
    simulation->max_frequency = max_frequency;
    simulation->cell_tolerance = cell_tolerance;
}
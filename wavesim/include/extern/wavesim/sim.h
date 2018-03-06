#ifndef SIMULATION_H
#define SIMULATION_H

#include "wavesim/config.h"
#include "wavesim/medium.h"
#include "wavesim/log.h"

C_BEGIN

typedef struct simulation_t simulation_t;
typedef wsret (*simulation_prepare_func)(simulation_t*);
typedef int (*simulation_advance_func)(simulation_t*,wsreal_t);
typedef void (*simulation_finalize_func)(simulation_t*);

typedef struct audio_source_t audio_source_t;
typedef struct audio_listener_t audio_listener_t;

/*! Opaque pointer, is implemented by the specific simulation */
typedef struct simulation_state_t simulation_state_t;

typedef enum simulation_type_e
{
    WS_ARD,
    WS_RAY
} simulation_type_e;

typedef struct simulation_t
{
    simulation_state_t* state;
    mesh_t* mesh;

    audio_source_t* audio_source;
    audio_listener_t* audio_listener;
    wsreal_t max_frequency;
    wsreal_t cell_tolerance;

    simulation_prepare_func prepare;
    simulation_advance_func advance;
    simulation_finalize_func finalize;
} simulation_t;

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
simulation_create(simulation_t** simulation, simulation_type_e type);

WAVESIM_PUBLIC_API void
simulation_destroy(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_construct(simulation_t* simulation, simulation_type_e type);

WAVESIM_PUBLIC_API void
simulation_destruct(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_set_type(simulation_t* simulation, simulation_type_e type);

WAVESIM_PUBLIC_API void
simulation_set_audio_source(simulation_t* simulation, audio_source_t* as);

WAVESIM_PUBLIC_API void
simulation_set_audio_listener(simulation_t* simulation, audio_listener_t* al);

WAVESIM_PUBLIC_API void
simulation_set_resolution(simulation_t* simulation, wsreal_t max_frequency, wsreal_t cell_tolerance);

WAVESIM_PUBLIC_API wsret
simulation_execute(simulation_t* simulation);

C_END

#endif /* SIMULATION_H */

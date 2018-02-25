#ifndef SIMULATION_H
#define SIMULATION_H

#include "wavesim/config.h"
#include "wavesim/medium.h"
#include "wavesim/log.h"

C_BEGIN

typedef struct simulation_state_t simulation_state_t;
typedef struct audio_source_t audio_source_t;
typedef struct audio_listener_t audio_listener_t;

typedef struct simulation_t
{
    medium_t* medium;
    audio_source_t* audio_source;
    audio_listener_t* audio_listener;
    simulation_state_t* state;

    wsreal_t cell_size;
    wsreal_t dt; /* Time step */
} simulation_t;

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
simulation_create(simulation_t** simulation);

WAVESIM_PUBLIC_API void
simulation_destroy(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_construct(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_destruct(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_set_medium(simulation_t* simulation, medium_t* medium);

WAVESIM_PUBLIC_API void
simulation_set_audio_source(simulation_t* simulation, audio_source_t* as);

WAVESIM_PUBLIC_API void
simulation_set_listener(simulation_t* simulation, audio_listener_t* al);

WAVESIM_PUBLIC_API void
simulation_set_max_frequency(simulation_t* simulation, wsreal_t hz);

WAVESIM_PUBLIC_API wsret
simulation_prepare(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_advance(simulation_t* state);

WAVESIM_PUBLIC_API void
simulation_finalize(simulation_t* simulation);

WAVESIM_PUBLIC_API wsret
simulation_execute(simulation_t* simulation);

C_END

#endif /* SIMULATION_H */

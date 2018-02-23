#ifndef SIMULATION_H
#define SIMULATION_H

#include "wavesim/config.h"
#include "wavesim/medium.h"
#include "wavesim/log.h"

C_BEGIN

typedef struct simulation_state_t simulation_state_t;

typedef struct simulation_t
{
    const medium_t* medium;
    simulation_state_t* state;
    wsreal_t cell_size;
} simulation_t;

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
simulation_create(simulation_t** simulation, const medium_t* medium);

WAVESIM_PUBLIC_API void
simulation_destroy(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_construct(simulation_t* simulation, const medium_t* medium);

WAVESIM_PUBLIC_API void
simulation_destruct(simulation_t* simulation);

WAVESIM_PUBLIC_API wsret
simulation_prepare(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_finalize(simulation_t* simulation);

WAVESIM_PUBLIC_API wsret
simulation_execute(simulation_t* simulation);

C_END

#endif /* SIMULATION_H */

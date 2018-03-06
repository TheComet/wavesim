#ifndef WAVESIM_SIMULATION_ARD_H
#define WAVESIM_SIMULATION_ARD_H

#include "wavesim/config.h"

C_BEGIN

typedef struct simulation_t simulation_t;

WAVESIM_PRIVATE_API wsret
simulation_ard_prepare(simulation_t* simulation);

WAVESIM_PRIVATE_API int
simulation_ard_advance(simulation_t* simulation, wsreal_t dt);

WAVESIM_PRIVATE_API void
simulation_ard_finalize(simulation_t* simulation);

C_END

#endif /* WAVESIM_SIMULATION_ARD_H */

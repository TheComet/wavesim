#ifndef WAVESIM_SIMULATION_RAY_H
#define WAVESIM_SIMULATION_RAY_H

#include "wavesim/config.h"

C_BEGIN

typedef struct simulation_t simulation_t;

WAVESIM_PRIVATE_API wsret
simulation_ray_prepare(simulation_t* simulation);

WAVESIM_PRIVATE_API int
simulation_ray_advance(simulation_t* simulation, wsreal_t dt);

WAVESIM_PRIVATE_API void
simulation_ray_finalize(simulation_t* simulation);

C_END

#endif /* WAVESIM_SIMULATION_RAY_H */

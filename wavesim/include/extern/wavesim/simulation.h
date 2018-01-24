#ifndef SIMULATION_H
#define SIMULATION_H

#include "wavesim/config.h"
#include "wavesim/partition.h"

C_BEGIN

typedef struct simulation_t
{
    WS_REAL max_frequency;
    int spatial_samples;
    partition_t partition;

} simulation_t;

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
simulation_create(simulation_t** simulation);

WAVESIM_PUBLIC_API void
simulation_destroy(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_construct(simulation_t* simulation);

WAVESIM_PUBLIC_API void
simulation_destruct(simulation_t* simulation);

C_END

#endif /* SIMULATION_H */

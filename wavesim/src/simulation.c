#include "wavesim/simulation.h"
#include "wavesim/memory.h"

/* ------------------------------------------------------------------------- */
wsret
simulation_create(simulation_t** simulation)
{
    *simulation = MALLOC(sizeof **simulation);
    if (*simulation == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    simulation_construct(*simulation);
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
simulation_construct(simulation_t* simulation)
{
    partition_construct(&simulation->partition);
}

/* ------------------------------------------------------------------------- */
void
simulation_destruct(simulation_t* simulation)
{
    partition_destruct(&simulation->partition);
}

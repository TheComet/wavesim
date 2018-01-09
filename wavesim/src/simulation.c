#include "wavesim/simulation.h"
#include "wavesim/memory.h"

/* ------------------------------------------------------------------------- */
simulation_t*
simulation_create(void)
{
    simulation_t* simulation = MALLOC(sizeof *simulation);
    if (simulation == NULL)
        OUT_OF_MEMORY(NULL);

    simulation_construct(simulation);
    return simulation;
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

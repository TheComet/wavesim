#include "wavesim/simulation_ray.h"

/* ------------------------------------------------------------------------- */
wsret
simulation_ray_prepare(simulation_t* simulation)
{
    (void)simulation;
    return WS_ERR_NOT_IMPLEMENTED; /* return WS_OK */
}

/* ------------------------------------------------------------------------- */
int
simulation_ray_advance(simulation_t* simulation, wsreal_t dt)
{
    (void)simulation;
    (void)dt;
    return 0; /* Return 1 as long as you need to continue simulation.
               * Return 0 to indicate simulation has ended successfully.
               * Return -1 if something went wrong during simulation. If this
               * happens, make sure to ws_log_data() an error message before
               * returning.
               */
}

/* ------------------------------------------------------------------------- */
void
simulation_ray_finalize(simulation_t* simulation)
{
    (void)simulation;
}

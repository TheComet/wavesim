#include "wavesim/wavesim.h"
#include "wavesim/memory.h"

/* ------------------------------------------------------------------------- */
void wavesim_init()
{
    memory_init();
}

/* ------------------------------------------------------------------------- */
void wavesim_deinit()
{
    memory_deinit();
}

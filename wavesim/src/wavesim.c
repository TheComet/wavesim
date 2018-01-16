#include "wavesim/wavesim.h"
#include "wavesim/memory.h"

/* ------------------------------------------------------------------------- */
int wavesim_init()
{
    return memory_init();
}

/* ------------------------------------------------------------------------- */
void wavesim_deinit()
{
    memory_deinit();
}

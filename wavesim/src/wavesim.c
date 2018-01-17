#include "wavesim/wavesim.h"
#include "wavesim/memory.h"

static int g_was_initialised = 0;

/* ------------------------------------------------------------------------- */
int wavesim_init()
{
    if (g_was_initialised)
        return 0;

    if (memory_init())
        return -1;

    g_was_initialised = 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
int wavesim_deinit()
{
    if (!g_was_initialised)
        return 0;

    g_was_initialised = 0;
    return memory_deinit();
}

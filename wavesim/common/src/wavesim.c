#include "wavesim/wavesim.h"
#include "wavesim/memory.h"
#include "wavesim/log.h"

static int g_was_initialised = 0;

/* ------------------------------------------------------------------------- */
wsret
wavesim_init(void)
{
    if (g_was_initialised)
    {
        ws_log_info(&g_ws_log, "WaveSim is already initialized, skipping...");
        return WS_OK;
    }

    if (memory_init())
        return WS_ERR_OUT_OF_MEMORY;

    log_construct(&g_ws_log);
    ws_log_info(&g_ws_log, "Initializing WaveSim...");

    g_was_initialised = 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
int
wavesim_deinit(void)
{
    if (!g_was_initialised)
        return 0;

    ws_log_info(&g_ws_log, "De-Initializing WaveSim...");
    log_destruct(&g_ws_log);

    g_was_initialised = 0;
    return memory_deinit();
}

/* ------------------------------------------------------------------------- */
int
wavesim_is_initialized(void)
{
    return g_was_initialised;
}

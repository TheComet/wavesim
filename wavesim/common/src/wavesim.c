#include "wavesim/wavesim.h"
#include "wavesim/memory.h"
#include "wavesim/log.h"
#include <stdio.h>

static int g_init_counter = 0;

/* ------------------------------------------------------------------------- */
wsret
wavesim_init(void)
{
    if (g_init_counter++ != 0)
        WSRET(WS_OK);

    if (memory_init() != 0)
    {
        g_init_counter--;
        fprintf(stderr, "%s\n", wsret_to_string(WS_ERR_OUT_OF_MEMORY));
        return WS_ERR_OUT_OF_MEMORY;
    }

    log_construct(&g_ws_log);
    ws_log_info(&g_ws_log, "Initializing WaveSim...");

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
int
wavesim_deinit(void)
{
    if (--g_init_counter != 0)
        return 0;

    ws_log_info(&g_ws_log, "De-Initializing WaveSim...");
    log_destruct(&g_ws_log);

    return memory_deinit();
}

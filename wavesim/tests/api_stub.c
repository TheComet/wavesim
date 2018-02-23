#include "wavesim/wavesim.h"
#include "wavesim/log.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
wsret
wavesim_run_tests(int* argc, char** argv)
{
    (void)argc;
    (void)argv;

    /*
     * Normally, the unit test library initializes wavesim, but since there are
     * no unit tests, we cannot rely on that. Use the log if it was initialized,
     * otherwise print the error to stderr.
     */
    if (wavesim_is_initialized() == 0)
    {
        fprintf(stderr, "Error: %s\n", wsret_to_string(WS_ERR_BUILT_WITHOUT_TESTS));
        return WS_ERR_BUILT_WITHOUT_TESTS;
    }
    WSRET(WS_ERR_BUILT_WITHOUT_TESTS);
}

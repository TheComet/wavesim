#include "wavesim/wavesim.h"
#include "stdio.h"

/* ------------------------------------------------------------------------- */
wsret
wavesim_run_tests(int* argc, char** argv)
{
    (void)argc;
    (void)argv;
    return WSRET(WS_ERR_BUILT_WITHOUT_TESTS);
}

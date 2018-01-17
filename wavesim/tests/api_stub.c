#include "wavesim/wavesim.h"
#include "stdio.h"

/* ------------------------------------------------------------------------- */
int
wavesim_run_tests(int* argc, char** argv)
{
    (void)argc;
    (void)argv;
    puts("Error: Library was built without unit tests. Try passing -DWAVESIM_TESTS=ON to CMake.");
    return -1;
}

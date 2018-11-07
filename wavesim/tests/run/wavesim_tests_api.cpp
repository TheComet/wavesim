#include "wavesim/wavesim.h"
#include <gmock/gmock.h>
#include <stdio.h>

wsret
wavesim_run_tests(int* argc, char** argv)
{
    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(argc, argv);

    wsret result;
    if ((result = wavesim_init()) != WS_OK)
        return result;

    result = WS_OK;
    if (RUN_ALL_TESTS() != 0)
        result = WS_ERR_UNIT_TESTS_FAILED;

    wavesim_deinit();
    fprintf(stderr, "%s\n", wsret_to_string(result));
    return result;
}

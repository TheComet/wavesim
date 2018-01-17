#include "wavesim/wavesim.h"
#include <gmock/gmock.h>

int
wavesim_run_tests(int* argc, char** argv)
{
    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(argc, argv);
    return RUN_ALL_TESTS();
}

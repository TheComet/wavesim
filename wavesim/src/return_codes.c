#include "wavesim/return_codes.h"
#include "wavesim/log.h"

static const char* codemap[] = {
    "No error.",
    "The program ran out of memory in a malloc() call somewhere.",
    "The library was built without unit tests. Try passing -DWAVESIM_TESTS=ON to CMake and rebuild.",
    "One or more unit tests failed to pass. This indicates that some bugs are present and need to be fixed. Consider running the unit tests with ./wavesim_tests --gtest_output=xml and submitting the resulting test_detail.xml file to https://github.com/thecomet/wavesim/issues",
    "An attempt was made to subdivide a node in the octree that was not a leaf node. This operation is only valid for leaf nodes.",
    "Failed to open file.",
    "Something went wrong while reading from a file/stream.",
    "3 indices were expected (to form a face), but there were less.",
    "A face that has more than 3 vertices was detected. Only triangular faces are supported.",
    "The corresponding index to a vertex was not found. This can occur in the obj exporter when the indices are exported and a vertex is not found in vi_map.",
    "Cannot do a simulation without a medium to simulate in. You need to create and pass a medium to the simulation  with simulation_set_medium()",
    "Simulation requires an audio source, but none was set.",
    "Simulation requires an audio listener, but none was set."
};

/* ------------------------------------------------------------------------- */
const char*
wsret_to_string(wsret code)
{
    return codemap[-code];
}

/* ------------------------------------------------------------------------- */
wsret wsret_wrapper(wsret code)
{
    if (code < 0)
        ws_log_info(&g_ws_log, "Error: %s", wsret_to_string(code));
    return code;
}

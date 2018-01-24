#include "wavesim/return_codes.h"
#include "wavesim/log.h"

static const char* codemap[] = {
    "No error.",
    "The program ran out of memory in a malloc() call somewhere.",
    "The library was built without unit tests. Try passing -DWAVESIM_TESTS=ON to CMake.",
    "One or more unit tests failed to pass. This indicates that some bugs are present and need to be fixed.",
    "An attempt was made to subdivide a node in the octree that was not a leaf node. This operation is only valid for leaf nodes."
};

/* ------------------------------------------------------------------------- */
const char*
wsret_to_string(wsret code)
{
    return codemap[-code];
}

/* ------------------------------------------------------------------------- */
wsret WSRET(wsret code)
{
    ws_log_info(&g_ws_log, wsret_to_string(code));
    return code;
}

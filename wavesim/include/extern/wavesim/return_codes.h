#ifndef WAVESIM_RETURN_CODES_H
#define WAVESIM_RETURN_CODES_H

#include "wavesim/config.h"

C_BEGIN

typedef enum wsret
{
    WS_OK                           = 0,
    WS_ERR_OUT_OF_MEMORY            = -1,
    WS_ERR_BUILT_WITHOUT_TESTS      = -2,
    WS_ERR_UNIT_TESTS_FAILED        = -3,
    WS_ERR_NOT_IMPLEMENTED          = -4,
    WS_ERR_SUBDIVIDED_NON_LEAF_NODE = -5,
    WS_ERR_FOPEN_FAILED             = -6,
    WS_ERR_READ_ERROR               = -7
} wsret;

WAVESIM_PUBLIC_API const char*
wsret_to_string(wsret code);

WAVESIM_PUBLIC_API wsret
wsret_wrapper(wsret code);

#define WSRET(code) return wsret_wrapper(code)

C_END

#endif /* WAVESIM_RETURN_CODES_H */

#ifndef WAVESIM_RETURN_CODES_H
#define WAVESIM_RETURN_CODES_H

#include "wavesim/config.h"

#define WSRET(code) return wsret_wrapper(code)

C_BEGIN

typedef enum wsret
{
    WS_KEY_EXISTS                     = 1,
    WS_OK                             = 0,
    WS_ERR_OUT_OF_MEMORY              = -1,
    WS_ERR_BUILT_WITHOUT_TESTS        = -2,
    WS_ERR_UNIT_TESTS_FAILED          = -3,
    WS_ERR_NOT_IMPLEMENTED            = -4,
    WS_ERR_SUBDIVIDED_NON_LEAF_NODE   = -5,
    WS_ERR_FOPEN_FAILED               = -6,
    WS_ERR_READ_ERROR                 = -7,
    WS_ERR_TOO_FEW_INDICES            = -8,
    WS_ERR_INDICES_ARENT_A_TRI        = -9,
    WS_ERR_VERTEX_INDEX_NOT_FOUND     = -10,
    WS_ERR_SIM_MEDIUM_NOT_SET         = -11,
    WS_ERR_SIM_AUDIO_SOURCE_NOT_SET   = -12,
    WS_ERR_SIM_AUDIO_LISTENER_NOT_SET = -13,
} wsret;

WAVESIM_PUBLIC_API int
wsret_is_error(wsret code);

WAVESIM_PUBLIC_API const char*
wsret_to_string(wsret code);

WAVESIM_PUBLIC_API wsret
wsret_wrapper(wsret code);

C_END

#endif /* WAVESIM_RETURN_CODES_H */

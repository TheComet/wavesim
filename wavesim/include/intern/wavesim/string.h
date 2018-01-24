#ifndef WAVESIM_STRING_H
#define WAVESIM_STRING_H

#include <stdio.h>
#include "wavesim/config.h"

C_BEGIN

WAVESIM_PRIVATE_API char*
ws_strtok(char* str, const char* delimiter, char** save_ptr);

C_END

#endif /* WAVESIM_STRING_H */

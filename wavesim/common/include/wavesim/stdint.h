#ifndef WAVESIM_STDINT_H
#define WAVESIM_STDINT_H

#include "wavesim/config.h"

#ifdef WAVESIM_HAVE_STDINT_H
#   include <stdint.h>
#else
#   include <wavesim/compat/pstdint.h>
#endif

#endif


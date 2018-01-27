#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "wavesim/config.h"

C_BEGIN

/*!
 * @brief 
 */
typedef struct attribute_t
{
    WS_REAL reflection;
    WS_REAL transmission;
    WS_REAL absorption;
} attribute_t;

WAVESIM_PRIVATE_API attribute_t
attribute(WS_REAL reflection, WS_REAL transmission, WS_REAL absorption);

WAVESIM_PRIVATE_API attribute_t
attribute_default(void);

WAVESIM_PRIVATE_API void
attribute_set_default(attribute_t* attribute);

WAVESIM_PRIVATE_API void
attribute_set_zero(attribute_t* attribute);

WAVESIM_PRIVATE_API int
attribute_is_same(const attribute_t* a1, const attribute_t* a2);

WAVESIM_PRIVATE_API void
attribute_normalize_rta(attribute_t* attribute);

C_END

#endif /* ATTRIBUTE_H */

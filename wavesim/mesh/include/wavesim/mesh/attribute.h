#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

/*!
 * @brief 
 */
typedef struct attribute_t
{
    /* Values between 0.0 and 1.0, where reflection + transmission + absorption = 1 */
    wsreal_t reflection;
    wsreal_t transmission;
    wsreal_t absorption;
    /*  */
    wsreal_t sound_velocity;
    vec3_t   velocity;
} attribute_t;

WAVESIM_PRIVATE_API attribute_t
attribute(wsreal_t reflection, wsreal_t transmission, wsreal_t absorption, wsreal_t sound_velocity, vec3_t velocity);

WAVESIM_PRIVATE_API attribute_t
attribute_default_solid(void);

WAVESIM_PRIVATE_API attribute_t
attribute_default_air(void);

WAVESIM_PRIVATE_API void
attribute_set_default_solid(attribute_t* attribute);

WAVESIM_PRIVATE_API void
attribute_set_default_air(attribute_t* attribute);

WAVESIM_PRIVATE_API void
attribute_set_zero(attribute_t* attribute);

WAVESIM_PRIVATE_API int
attribute_is_zero(attribute_t* attribute);

WAVESIM_PRIVATE_API int
attribute_is_same(const attribute_t* a1, const attribute_t* a2);

WAVESIM_PRIVATE_API void
attribute_normalize_rta(attribute_t* attribute);

C_END

#endif /* ATTRIBUTE_H */

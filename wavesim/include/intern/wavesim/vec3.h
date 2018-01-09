#ifndef VEC3_H
#define VEC3_H

#include "wavesim/config.h"

C_BEGIN

typedef union vec3_t
{
    struct {
        real x;
        real y;
        real z;
    } v;
    real f[3];
} vec3_t;

WAVESIM_PRIVATE_API vec3_t
vec3(real x, real y, real z);

WAVESIM_PRIVATE_API void
vec3_set_zero(real* v);

WAVESIM_PRIVATE_API void
vec3_add_vec3(real* v1, const real* v2);

WAVESIM_PRIVATE_API void
vec3_sub_vec3(real* v1, const real* v2);

WAVESIM_PRIVATE_API void
vec3_mul_scalar(real* v1, real scalar);

WAVESIM_PRIVATE_API void
vec3_div_scalar(real* v, real scalar);

WAVESIM_PRIVATE_API real
vec3_length_squared(const real* v);

WAVESIM_PRIVATE_API real
vec3_length(const real* v);

WAVESIM_PRIVATE_API void
vec3_normalise(real* v);

WAVESIM_PRIVATE_API real
vec3_dot(const real* v1, const real* v2);

WAVESIM_PRIVATE_API void
vec3_cross(real* v1, const real* v2);

C_END

#endif /* VEC3_H */

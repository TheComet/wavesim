#ifndef VEC3_H
#define VEC3_H

#include "wavesim/config.h"

C_BEGIN

typedef union vec3_t
{
    struct {
        WS_REAL x;
        WS_REAL y;
        WS_REAL z;
    } data;
    WS_REAL xyz[3];
} vec3_t;

WAVESIM_PRIVATE_API vec3_t
vec3(WS_REAL x, WS_REAL y, WS_REAL z);

WAVESIM_PRIVATE_API void
vec3_set_zero(WS_REAL v[3]);

WAVESIM_PRIVATE_API int
vec3_is_zero(const WS_REAL v[3]);

WAVESIM_PRIVATE_API void
vec3_copy(vec3_t* dst, const WS_REAL src[3]);

WAVESIM_PRIVATE_API void
vec3_add_vec3(WS_REAL v1[3], const WS_REAL v2[3]);

WAVESIM_PRIVATE_API void
vec3_sub_vec3(WS_REAL v1[3], const WS_REAL v2[3]);

WAVESIM_PRIVATE_API void
vec3_mul_scalar(WS_REAL v1[3], WS_REAL scalar);

WAVESIM_PRIVATE_API void
vec3_div_scalar(WS_REAL v[3], WS_REAL scalar);

WAVESIM_PRIVATE_API WS_REAL
vec3_length_squared(const WS_REAL v[3]);

WAVESIM_PRIVATE_API WS_REAL
vec3_length(const WS_REAL v[3]);

WAVESIM_PRIVATE_API void
vec3_normalise(WS_REAL v[3]);

WAVESIM_PRIVATE_API WS_REAL
vec3_dot(const WS_REAL v1[3], const WS_REAL v2[3]);

WAVESIM_PRIVATE_API void
vec3_cross(WS_REAL v1[3], const WS_REAL v2[3]);

C_END

#endif /* VEC3_H */

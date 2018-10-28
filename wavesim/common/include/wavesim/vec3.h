#ifndef VEC3_H
#define VEC3_H

#include "wavesim/config.h"

C_BEGIN

typedef union vec3_t
{
    struct {
        wsreal_t x;
        wsreal_t y;
        wsreal_t z;
    } v;
    wsreal_t xyz[3];
} vec3_t;

WAVESIM_PRIVATE_API vec3_t
vec3(wsreal_t x, wsreal_t y, wsreal_t z);

WAVESIM_PRIVATE_API void
vec3_set_zero(wsreal_t v[3]);

WAVESIM_PRIVATE_API int
vec3_is_zero(const wsreal_t v[3]);

WAVESIM_PRIVATE_API void
vec3_copy(vec3_t* dst, const wsreal_t src[3]);

WAVESIM_PRIVATE_API void
vec3_add_vec3(wsreal_t v1[3], const wsreal_t v2[3]);

WAVESIM_PRIVATE_API void
vec3_sub_vec3(wsreal_t v1[3], const wsreal_t v2[3]);

WAVESIM_PRIVATE_API void
vec3_mul_scalar(wsreal_t v1[3], wsreal_t scalar);

WAVESIM_PRIVATE_API void
vec3_div_scalar(wsreal_t v[3], wsreal_t scalar);

WAVESIM_PRIVATE_API wsreal_t
vec3_length_squared(const wsreal_t v[3]);

WAVESIM_PRIVATE_API wsreal_t
vec3_length(const wsreal_t v[3]);

WAVESIM_PRIVATE_API void
vec3_normalize(wsreal_t v[3]);

WAVESIM_PRIVATE_API wsreal_t
vec3_dot(const wsreal_t v1[3], const wsreal_t v2[3]);

WAVESIM_PRIVATE_API void
vec3_cross(wsreal_t v1[3], const wsreal_t v2[3]);

C_END

#endif /* VEC3_H */

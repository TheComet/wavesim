#include "wavesim/vec3.h"
#include <string.h>
#include <math.h>

/* ------------------------------------------------------------------------- */
vec3_t
vec3(WS_REAL x, WS_REAL y, WS_REAL z)
{
    vec3_t v;
    v.v.x = x;
    v.v.y = y;
    v.v.z = z;
    return v;
}

/* ------------------------------------------------------------------------- */
void
vec3_set_zero(WS_REAL v[3])
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

/* ------------------------------------------------------------------------- */
int
vec3_is_zero(const WS_REAL v[3])
{
    return (v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0);
}

/* ------------------------------------------------------------------------- */
void
vec3_copy(vec3_t* dst, const WS_REAL src[3])
{
    memcpy(dst->xyz, src, sizeof(WS_REAL) * 3);
}

/* ------------------------------------------------------------------------- */
void
vec3_add_vec3(WS_REAL v1[3], const WS_REAL v2[3])
{
    v1[0] += v2[0];
    v1[1] += v2[1];
    v1[2] += v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_sub_vec3(WS_REAL v1[3], const WS_REAL v2[3])
{
    v1[0] -= v2[0];
    v1[1] -= v2[1];
    v1[2] -= v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_mul_scalar(WS_REAL v[3], WS_REAL scalar)
{
    v[0] *= scalar;
    v[1] *= scalar;
    v[2] *= scalar;
}

/* ------------------------------------------------------------------------- */
void
vec3_div_scalar(WS_REAL v[3], WS_REAL scalar)
{
    v[0] /= scalar;
    v[1] /= scalar;
    v[2] /= scalar;
}

/* ------------------------------------------------------------------------- */
WS_REAL
vec3_length_squared(const WS_REAL v[3])
{
    return vec3_dot(v, v);
}

/* ------------------------------------------------------------------------- */
WS_REAL
vec3_length(const WS_REAL v[3])
{
    return sqrt(vec3_length_squared(v));
}

/* ------------------------------------------------------------------------- */
void
vec3_normalize(WS_REAL v[3])
{
    WS_REAL length = vec3_length(v);
    if (length != 0.0)
    {
        length = 1.0 / length;
        v[0] *= length;
        v[1] *= length;
        v[2] *= length;
    }
    else
    {
        v[0] = 1;
    }
}

/* ------------------------------------------------------------------------- */
WS_REAL
vec3_dot(const WS_REAL v1[3], const WS_REAL v2[3])
{
    return v1[0] * v2[0] +
           v1[1] * v2[1] +
           v1[2] * v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_cross(WS_REAL v1[3], const WS_REAL v2[3])
{
    WS_REAL v1x = v1[1] * v2[2] - v2[1] * v1[2];
    WS_REAL v1z = v1[0] * v2[1] - v2[0] * v1[1];
    v1[1]    = v1[2] * v2[0] - v2[2] * v1[0];
    v1[0] = v1x;
    v1[2] = v1z;
}

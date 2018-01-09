#include "wavesim/vec3.h"
#include <string.h>
#include <math.h>

/* ------------------------------------------------------------------------- */
vec3_t
vec3(real x, real y, real z)
{
    return (vec3_t){{x, y, z}};
}

/* ------------------------------------------------------------------------- */
void
vec3_set_zero(real* RESTRICT v)
{
    memset(v, 0, sizeof *v);
}

/* ------------------------------------------------------------------------- */
void
vec3_add_vec3(real* RESTRICT v1, const real* RESTRICT v2)
{
    v1[0] += v2[0];
    v1[1] += v2[1];
    v1[2] += v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_sub_vec3(real* RESTRICT v1, const real* RESTRICT v2)
{
    v1[0] -= v2[0];
    v1[1] -= v2[1];
    v1[2] -= v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_mul_scalar(real* RESTRICT v, real scalar)
{
    v[0] *= scalar;
    v[1] *= scalar;
    v[2] *= scalar;
}

/* ------------------------------------------------------------------------- */
void
vec3_div_scalar(real* RESTRICT v, real scalar)
{
    v[0] /= scalar;
    v[1] /= scalar;
    v[2] /= scalar;
}

/* ------------------------------------------------------------------------- */
real
vec3_length_squared(const real* RESTRICT v)
{
    return vec3_dot(v, v);
}

/* ------------------------------------------------------------------------- */
real
vec3_length(const real* RESTRICT v)
{
    return sqrt(vec3_length_squared(v));
}

/* ------------------------------------------------------------------------- */
void
vec3_normalise(real* RESTRICT v)
{
    real length = vec3_length(v);
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
real
vec3_dot(const real* RESTRICT v1, const real* RESTRICT v2)
{
    return v1[0] * v2[0] +
           v1[1] * v2[1] +
           v1[2] * v2[2];
}

/* ------------------------------------------------------------------------- */
void
vec3_cross(real* RESTRICT v1, const real* RESTRICT v2)
{
    real v1x = v1[1] * v2[2] - v2[1] * v1[2];
    real v1z = v1[0] * v2[1] - v2[0] * v1[1];
    v1[1]       = v1[2] * v2[0] - v2[2] * v1[0];
    v1[0] = v1x;
    v1[2] = v1z;
}

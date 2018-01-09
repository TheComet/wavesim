#ifndef AABB_H
#define AABB_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef struct face_t face_t;

typedef struct aabb_t
{
    vec3_t a;
    vec3_t b;
} aabb_t;

WAVESIM_PRIVATE_API aabb_t
aabb(real ax, real ay, real az, real bx, real by, real bz);

WAVESIM_PRIVATE_API aabb_t
aabb_reset(void);

WAVESIM_PRIVATE_API int
aabb_point_is_inside(const aabb_t* aabb, vec3_t point);

WAVESIM_PRIVATE_API int
aabb_face_is_inside(const aabb_t* aabb, const face_t* face);

C_END

#endif /* AABB_H */

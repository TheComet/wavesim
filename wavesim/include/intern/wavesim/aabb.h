#ifndef AABB_H
#define AABB_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef struct face_t face_t;

typedef union aabb_t
{
    struct {
        vec3_t a;
        vec3_t b;
    } data;
    WS_REAL xyzxyz[6];
} aabb_t;

#define AABB_AX(X) X.data.a.data.x
#define AABB_AY(X) X.data.a.data.y
#define AABB_AZ(X) X.data.a.data.z
#define AABB_BX(X) X.data.b.data.x
#define AABB_BY(X) X.data.b.data.y
#define AABB_BZ(X) X.data.b.data.z

#define AABB_DIMS(X) \
    vec3(AABB_BX(X) - AABB_AX(X), \
         AABB_BY(X) - AABB_BY(X), \
         AABB_BZ(X) - AABB_BZ(X))

WAVESIM_PRIVATE_API aabb_t
aabb(WS_REAL ax, WS_REAL ay, WS_REAL az, WS_REAL bx, WS_REAL by, WS_REAL bz);

WAVESIM_PRIVATE_API aabb_t
aabb_reset(void);

WAVESIM_PRIVATE_API aabb_t
aabb_from_3_points(const WS_REAL p1[3], const WS_REAL p2[3], const WS_REAL p3[3]);

C_END

#endif /* AABB_H */

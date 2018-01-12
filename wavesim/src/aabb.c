#include "wavesim/aabb.h"
#include "wavesim/face.h"
#include "math.h"

/* ------------------------------------------------------------------------- */
aabb_t
aabb(WS_REAL ax, WS_REAL ay, WS_REAL az, WS_REAL bx, WS_REAL by, WS_REAL bz)
{
    return (aabb_t){{
        {{ax, ay, az}},
        {{bx, by, bz}},
    }};
}

/* ------------------------------------------------------------------------- */
aabb_t
aabb_reset(void)
{
    return aabb(INFINITY, INFINITY, INFINITY, -INFINITY, -INFINITY, -INFINITY);
}

/* ------------------------------------------------------------------------- */
aabb_t
aabb_from_3_points(const WS_REAL p1[3], const WS_REAL p2[3], const WS_REAL p3[3])
{
    int i;
    aabb_t ret = aabb_reset();
    for (i = 0; i != 3; ++i)
    {
        if (p1[i] < ret.data.a.xyz[i])
            ret.data.a.xyz[i] = p1[i];
        if (p1[i] > ret.data.b.xyz[i])
            ret.data.b.xyz[i] = p1[i];

        if (p2[i] < ret.data.a.xyz[i])
            ret.data.a.xyz[i] = p1[i];
        if (p2[i] > ret.data.b.xyz[i])
            ret.data.b.xyz[i] = p2[i];

        if (p3[i] < ret.data.a.xyz[i])
            ret.data.a.xyz[i] = p3[i];
        if (p3[i] > ret.data.b.xyz[i])
            ret.data.b.xyz[i] = p3[i];
    }

    return ret;
}

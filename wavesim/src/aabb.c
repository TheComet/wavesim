#include "wavesim/aabb.h"
#include "wavesim/face.h"
#include "math.h"
#include "assert.h"

/* ------------------------------------------------------------------------- */
aabb_t
aabb(WS_REAL ax, WS_REAL ay, WS_REAL az, WS_REAL bx, WS_REAL by, WS_REAL bz)
{
    assert(ax <= bx);
    assert(ay <= by);
    assert(az <= bz);
    return (aabb_t){{
        {{ax, ay, az}},
        {{bx, by, bz}},
    }};
}

/* ------------------------------------------------------------------------- */
aabb_t
aabb_reset(void)
{
    return (aabb_t){{
        {{INFINITY, INFINITY, INFINITY}},
        {{-INFINITY, -INFINITY, -INFINITY}},
    }};
}

/* ------------------------------------------------------------------------- */
aabb_t
aabb_from_3_points(const WS_REAL p1[3], const WS_REAL p2[3], const WS_REAL p3[3])
{
    aabb_t ret = aabb(
        fmin(p1[0], p2[0]),
        fmin(p1[1], p2[1]),
        fmin(p1[2], p2[2]),
        fmax(p1[0], p2[0]),
        fmax(p1[1], p2[1]),
        fmax(p1[2], p2[2])
    );
    aabb_expand_point(ret.xyzxyz, p3);

    return ret;
}

/* ------------------------------------------------------------------------- */
void
aabb_expand_point(WS_REAL aabb[6], const WS_REAL p[3])
{
    int i;
    for (i = 0; i != 3; ++i)
    {
        if (p[i] < aabb[i])
            aabb[i] = p[i];
        if (p[i] > aabb[i+3])
            aabb[i+3] = p[i];
    }
}

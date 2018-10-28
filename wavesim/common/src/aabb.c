#include "wavesim/aabb.h"
#include "wavesim/mesh/face.h"
#include <math.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
aabb_t
aabb(wsreal_t ax, wsreal_t ay, wsreal_t az, wsreal_t bx, wsreal_t by, wsreal_t bz)
{
    assert(ax <= bx);
    assert(ay <= by);
    assert(az <= bz);

    aabb_t bb;
    AABB_AX(bb) = ax;
    AABB_AY(bb) = ay;
    AABB_AZ(bb) = az;
    AABB_BX(bb) = bx;
    AABB_BY(bb) = by;
    AABB_BZ(bb) = bz;
    return bb;
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
aabb_zero(void)
{
    return (aabb_t){{{{0, 0, 0}}, {{0, 0, 0}}}};
}

/* ------------------------------------------------------------------------- */
aabb_t
aabb_from_3_points(const wsreal_t p1[3], const wsreal_t p2[3], const wsreal_t p3[3])
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
aabb_t
aabb_from_face(const face_t* face)
{
    return aabb_from_3_points(face->vertices[0].position.xyz,
                              face->vertices[1].position.xyz,
                              face->vertices[2].position.xyz);
}

/* ------------------------------------------------------------------------- */
void
aabb_expand_point(wsreal_t aabb[6], const wsreal_t p[3])
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

/* ------------------------------------------------------------------------- */
void
aabb_expand_aabb(wsreal_t aabb[6], const wsreal_t aabb_other[6])
{
    aabb_expand_point(aabb, &aabb_other[0]);
    aabb_expand_point(aabb, &aabb_other[3]);
}

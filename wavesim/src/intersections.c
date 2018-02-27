#include "wavesim/intersections.h"
#include <string.h>
#include <math.h>
#include <float.h>

/* ------------------------------------------------------------------------- */
int
intersect_point_aabb_test(const wsreal_t point[3], const wsreal_t aabb[6])
{
    return (
        point[0] >= aabb[0] && point[0] <= aabb[3] &&
        point[1] >= aabb[1] && point[1] <= aabb[4] &&
        point[2] >= aabb[2] && point[2] <= aabb[5]
    );
}

/* ------------------------------------------------------------------------- */
int
intersect_aabb_aabb_test(const wsreal_t aabb1[6], const wsreal_t aabb2[6])
{
    int i;
    for (i = 0; i != 3; ++i)
        if (aabb1[i+3] <= aabb2[i+0] || aabb1[i+0] >= aabb2[i+3]) return 0;
    return 1;
}

/* ------------------------------------------------------------------------- */
/*!
 * Behaves the same as intersect_line_plane() (see intersections.h), except
 * it also writes the triangle edge vectors "u,v". This is used in the
 * line-triangle intersection test below so u and v don't have to be calculated
 * twice.
 */
static int
intersect_line_plane_internal(wsreal_t result[3], wsreal_t u[3], wsreal_t v[3],
                              const wsreal_t p0[3], const wsreal_t p1[3],
                              const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3])
{
    vec3_t n, w0;
    wsreal_t a, b, r;

    /*
     * Algorithm taken from:
     * http://geomalgorithms.com/a06-_intersect-2.html
     */

    /* Get triangle edge vectors "u,v" and plane normal "n" */
    memcpy(u, v1, sizeof(wsreal_t)*3);
    memcpy(v, v2, sizeof(wsreal_t)*3);
    vec3_sub_vec3(u, v0);
    vec3_sub_vec3(v, v0);
    vec3_copy(&n, u);
    vec3_cross(n.xyz, v); /* plane normal */

    /* Not checking for degenerate triangle, but could do so if n == 0 */

    /* Calculate ray direction vector, store it in "result" */
    memcpy(result, p1, sizeof(wsreal_t)*3);
    vec3_copy(&w0, p0);
    vec3_sub_vec3(result, p0);
    vec3_sub_vec3(w0.xyz, v0);

    a = -vec3_dot(n.xyz, w0.xyz);
    b = vec3_dot(n.xyz, result);
    if (fabs(b) < WS_EPSILON) /* ray is parallel to triangle plane */
        return 0;

    /* Get intersection point of ray with triangle */
    r = a / b;
    if (r < 0.0) /* ray goes away from triangle -> no intersect */
        return 0;
    if (r > 1.0) /* ray is not long enough to reach triangle (comment out for ray-triangle) */
        return 0;
    vec3_mul_scalar(result, r);
    vec3_add_vec3(result, p0);

    return 1;
}
int
intersect_line_plane(wsreal_t result[3],
                     const wsreal_t p0[3], const wsreal_t p1[3],
                     const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3])
{
    vec3_t u, v;
    return intersect_line_plane_internal(result, u.xyz, v.xyz, p0, p1, v0, v1, v2);
}

/* ------------------------------------------------------------------------- */
int
intersect_plane_aabb_test(const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3],
                          const wsreal_t aabb[6])
{
    vec3_t u, n, c, e;
    wsreal_t r, s, d;

    /* calculate AABB center */
    vec3_copy(&c, aabb+3);
    vec3_add_vec3(c.xyz, aabb);
    vec3_mul_scalar(c.xyz, 0.5);

    /* Calculate positive extents */
    vec3_copy(&e, aabb+3);
    vec3_sub_vec3(e.xyz, c.xyz);

    /* Calculate plane normal */
    vec3_copy(&u, v1); vec3_sub_vec3(u.xyz, v0);
    vec3_copy(&n, v2); vec3_sub_vec3(n.xyz, v0);
    vec3_cross(n.xyz, u.xyz);

    /* Calculate "d" of plane equation by plugging in one vertex into ax+bx+cx=d */
    d = vec3_dot(n.xyz, v0);

    /* Calculate the projection interval radius of b onto L(t) = b.c + t*p.n */
    r = e.v.x*fabs(n.v.x) + e.v.y*fabs(n.v.y) + e.v.z*fabs(n.v.z);
    /* Calculate distance of box center from plane */
    s = vec3_dot(n.xyz, c.xyz) - d;
    /* Intersection occurs when distance s falls within [-r,+r] interval */
    return fabs(s) <= r;
}

/* ------------------------------------------------------------------------- */
int
intersect_line_triangle_barycentric(wsreal_t result[3],
                                    const wsreal_t p0[3], const wsreal_t p1[3],
                                    const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3])
{
    vec3_t u, v;
    wsreal_t determinant, uu, vv, uv, wu, wv;

    /*
     * Algorithm taken from:
     * http://geomalgorithms.com/a06-_intersect-2.html
     */

    if (intersect_line_plane_internal(result, u.xyz, v.xyz, p0, p1, v0, v1, v2) == 0)
        return 0;

    /* Cache dot products (note: "w" is "result")*/
    vec3_sub_vec3(result, v0);
    uu = vec3_dot(u.xyz, u.xyz);
    vv = vec3_dot(v.xyz, v.xyz);
    uv = vec3_dot(u.xyz, v.xyz);
    wu = vec3_dot(result, u.xyz);
    wv = vec3_dot(result, v.xyz);

    /* Calculate determinant */
    determinant = uv*uv - uu*vv;

    /* Calculate parametric coordinates and test */
    result[0] = (uv*wv - vv*wu) / determinant;
    if (result[0] < 0.0 || result[0] > 1.0) /* intersect point is outside of triangle */
        return 0;
    result[1] = (uv*wu - uu*wv) / determinant;
    if (result[1] < 0.0 || result[0]+result[1] > 1.0)  /* intersect point is outside of triangle */
        return 0;

    /* Calculate remaining barycentric coordinate */
    result[2] = 1.0 - result[1] - result[0];

    return 1;
}

/* ------------------------------------------------------------------------- */
int
intersect_line_triangle_cartesian(wsreal_t result[3],
                                  const wsreal_t p0[3], const wsreal_t p1[3],
                                  const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3])
{
    vec3_t u, v, w;
    wsreal_t determinant, uu, vv, uv, wu, wv, s, t;

    /*
     * Algorithm taken from:
     * http://geomalgorithms.com/a06-_intersect-2.html
     */

    if (intersect_line_plane_internal(result, u.xyz, v.xyz, p0, p1, v0, v1, v2) == 0)
        return 0;

    /* Cache dot products */
    vec3_copy(&w, result);
    vec3_sub_vec3(w.xyz, v0);
    uu = vec3_dot(u.xyz, u.xyz);
    vv = vec3_dot(v.xyz, v.xyz);
    uv = vec3_dot(u.xyz, v.xyz);
    wu = vec3_dot(w.xyz, u.xyz);
    wv = vec3_dot(w.xyz, v.xyz);

    /* Calculate determinant */
    determinant = uv*uv - uu*vv;

    /* Calculate parametric coordinates and test */
    s = (uv*wv - vv*wu) / determinant;
    if (s < 0.0 || s > 1.0) /* intersect point is outside of triangle */
        return 0;
    t = (uv*wu - uu*wv) / determinant;
    if (t < 0.0 || s+t > 1.0)  /* intersect point is outside of triangle */
        return 0;

    return 1;
}

/* ------------------------------------------------------------------------- */
int
intersect_line_triangle_test(const wsreal_t p0[3], const wsreal_t p1[3],
                             const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3])
{
    vec3_t result;
    return intersect_line_triangle_barycentric(result.xyz, p0, p1, v0, v1, v2);
}

/* ------------------------------------------------------------------------- */
int
intersect_triangle_aabb_test(const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3],
                             const wsreal_t aabb[6])
{
    vec3_t c, v0c, v1c, v2c, f0, f1, f2;
    wsreal_t e0, e1, e2, p0, p1, p2, r;

    /* Calculate box center */
    vec3_copy(&c, aabb+3); /* copies second 3 elements of aabb, max */
    vec3_add_vec3(c.xyz, aabb);
    vec3_mul_scalar(c.xyz, 0.5);;

    /* Calculate box extents */
    e0 = (aabb[3] - aabb[0]) * 0.5;
    e1 = (aabb[4] - aabb[1]) * 0.5;
    e2 = (aabb[5] - aabb[2]) * 0.5;

    /* Translate triangle as conceptually moving AABB to origin */
    vec3_copy(&v0c, v0); vec3_sub_vec3(v0c.xyz, c.xyz);
    vec3_copy(&v1c, v1); vec3_sub_vec3(v1c.xyz, c.xyz);
    vec3_copy(&v2c, v2); vec3_sub_vec3(v2c.xyz, c.xyz);

    /* Calculate edge vectors of triangle */
    vec3_copy(&f0, v1); vec3_sub_vec3(f0.xyz, v0);
    vec3_copy(&f1, v2); vec3_sub_vec3(f1.xyz, v1);
    vec3_copy(&f2, v0); vec3_sub_vec3(f2.xyz, v2);

    /*
     * n00 = u0 x f0 = (1,0,0) x f0 = (0,-f0z,f0y)
     * n01 = u0 x f1 = (1,0,0) x f1 = (0,-f1z,f1y)
     * n02 = u0 x f2 = (1,0,0) x f2 = (0,-f2z,f2y)
     * n10 = u1 x f0 = (0,1,0) x f0 = (f0z,0,-f0x)
     * n11 = u1 x f1 = (0,1,0) x f1 = (f1z,0,-f1x)
     * n12 = u1 x f2 = (0,1,0) x f2 = (f2z,0,-f2x)
     * n20 = u2 x f0 = (0,0,1) x f0 = (-f0y,f0x,0)
     * n21 = u2 x f1 = (0,0,1) x f1 = (-f1y,f1x,0)
     * n22 = u2 x f2 = (0,0,1) x f2 = (-f2y,f2x,0)
     *
     * p0 = v0*n
     * p1 = v1*n
     * p2 = v2*n
     * r = e0|u0*n| + e1|u1*n| + e2|u2*n|
     */

    /* n00: p0=v0z*v1y - v0y*v1z, p2=v2z*f0y - v2y*f0z */
    p0 = v0c.v.z*v1c.v.y - v0c.v.y*v1c.v.z;
    p2 = v2c.v.z*f0.v.y - v2c.v.z*f0.v.z;
    r = e1*fabs(f0.v.z) + e2*fabs(f0.v.y);
    if (fmax(p0, p2) < -r || fmin(p0, p2) > r) return 0; /* Axis is a separating axis */
    /* n01: p0=v0z*f1y - v0y*f1z, p1=v2y*v1z - v1y*v2z */
    p0 = v0c.v.z*f1.v.y - v0c.v.y*f1.v.z;
    p1 = v2c.v.y*v1c.v.z - v1c.v.y*v2c.v.z;
    r = e1*fabs(f1.v.z) + e2*fabs(f1.v.y);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;
    /* n02: p0=v0y*v2z - v2y*v0z, p1=v1z*f2y - v1y*f2z */
    p0 = v0c.v.y*v2c.v.z - v2c.v.y*v0c.v.z;
    p1 = v1c.v.z*f2.v.y - v1c.v.y*f2.v.z;
    r = e1*fabs(f2.v.z) + e2*fabs(f2.v.y);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;
    /* n10: p0=v0x*v1z - v1x*v0z, p2=v2z*f0x - v2x*f0z */
    p0 = v0c.v.x*v1c.v.z - v1c.v.x*v0c.v.z;
    p2 = v2c.v.z*f0.v.x - v2c.v.x*f0.v.z;
    r = e0*fabs(f0.v.z) + e2*fabs(f0.v.x);
    if (fmax(p0, p2) < -r || fmin(p0, p2) > r) return 0;
    /* n11: p0=v0z*f1x - v0x*f1z, p1=v1x*v2z - v2x*v1z */
    p0 = v0c.v.z*f1.v.x - v0c.v.x*f1.v.z;
    p1 = v1c.v.x*v2c.v.z - v2c.v.x*v1c.v.z;
    r = e0*fabs(f1.v.z) + e2*fabs(f1.v.x);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;
    /* n12: p0=v2x*v0z - v0x*v2z, p1=v1x*f2z - v1z*f2x */
    p0 = v2c.v.x*v0c.v.z - v0c.v.x*v2c.v.z;
    p1 = v1c.v.x*f2.v.z - v1c.v.z*f2.v.x;
    r = e0*fabs(f2.v.z) + e2*fabs(f2.v.x);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;
    /* n20: p0=v1x*v0y - v0x*v1y, p2=v2x*f0y - v2y*f0x */
    p0 = v1c.v.x*v0c.v.y - v0c.v.x*v1c.v.y;
    p2 = v2c.v.x*f0.v.y - v2c.v.y*f0.v.x;
    r = e0*fabs(f0.v.y) + e1*fabs(f0.v.x);
    if (fmax(p0, p2) < -r || fmin(p0, p2) > r) return 0;
    /* n21: p0=v0x*f1y - v0y*f1x, p1=v2x*v1y - v1x*v2y */
    p0 = v0c.v.x*f1.v.y - v0c.v.y*f1.v.x;
    p1 = v2c.v.x*v1c.v.y - v1c.v.x*v2c.v.y;
    r = e0*fabs(f1.v.y) + e1*fabs(f1.v.x);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;
    /* n22: p0=v0x*v2y - v2x*v0y, p1=v1y*f2x - v1x*f2y */
    p0 = v0c.v.x*v2c.v.y - v2c.v.x*v0c.v.y;
    p1 = v1c.v.y*f2.v.x - v1c.v.x*f2.v.y;
    r = e0*fabs(f2.v.y) + e1*fabs(f2.v.x);
    if (fmax(p0, p1) < -r || fmin(p0, p1) > r) return 0;

    /* Test the three axes corresponding to the face normals of AABB b */
    /* Exit if... */
    /* ... [-e0,e0] and [min(v1x,v2x,v3x), max(v1x,v2x,v3x)] do not overlap */
    if (fmax(fmax(v0c.v.x, v1c.v.x), v2c.v.x) < -e0 || fmin(fmin(v0c.v.x, v1c.v.x), v2c.v.x) > e0) return 0;
    /* ... [-e1,e1] and [min(v1y,v2y,v3y), max(v1y,v2y,v3y)] do not overlap */
    if (fmax(fmax(v0c.v.y, v1c.v.y), v2c.v.y) < -e1 || fmin(fmin(v0c.v.y, v1c.v.y), v2c.v.y) > e1) return 0;
    /* ... [-e2,e2] and [min(v1z,v2z,v3z), max(v1z,v2z,v3z)] do not overlap */
    if (fmax(fmax(v0c.v.z, v1c.v.z), v2c.v.z) < -e2 || fmin(fmin(v0c.v.z, v1c.v.z), v2c.v.z) > e2) return 0;

    /* Test separating axis corresponding to triangle face normal */
    return intersect_plane_aabb_test(v0, v1, v2, aabb);
}

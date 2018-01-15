#include "wavesim/intersections.h"
#include "wavesim/face.h"
#include <string.h>
#include <math.h>
#include <float.h>

/* ------------------------------------------------------------------------- */
int
intersect_point_aabb_test(const WS_REAL point[3], const WS_REAL aabb[6])
{
    return (
        point[0] >= aabb[0] && point[0] <= aabb[3] &&
        point[1] >= aabb[1] && point[1] <= aabb[4] &&
        point[2] >= aabb[2] && point[2] <= aabb[5]
    );
}

/* ------------------------------------------------------------------------- */
int
intersect_aabb_aabb_test(const WS_REAL aabb1[6], const WS_REAL aabb2[6])
{
    return (
        aabb1[3] < aabb2[0] &&
        aabb1[4] < aabb2[1] &&
        aabb1[5] < aabb2[2] &&
        aabb1[0] > aabb2[3] &&
        aabb1[1] > aabb2[4] &&
        aabb1[2] > aabb2[5]
    );
}

/* ------------------------------------------------------------------------- */
int
intersect_line_face(intersect_result_t* result, const WS_REAL p1[3], const WS_REAL p2[3], const face_t* face)
{
    vec3_t u, v, n, w, dir, w0, intersect;
    WS_REAL determinant, a, b, r, uu, vv, uv, wu, wv, s, t;
    result->count = 0;

    /*
     * Algorithm taken from:
     * http://geomalgorithms.com/a06-_intersect-2.html
     */

    /* Get face edge vectors and plane normal */
    u = face->vertices[1].position;
    vec3_sub_vec3(u.xyz, face->vertices[0].position.xyz);
    v = face->vertices[2].position;
    vec3_sub_vec3(v.xyz, face->vertices[0].position.xyz);
    n = u;
    vec3_cross(n.xyz, v.xyz);

    /* Not checking for degenerate triangle, but could do so if n == 0 */

    /* Calculate ray direction vector */
    memcpy(dir.xyz, p2, sizeof(WS_REAL) * 3);
    memcpy(w0.xyz, p1, sizeof(WS_REAL) * 3);
    vec3_sub_vec3(dir.xyz, p1);
    vec3_sub_vec3(w0.xyz, face->vertices[0].position.xyz);

    a = -vec3_dot(n.xyz, w0.xyz);
    b = vec3_dot(n.xyz, dir.xyz);
    if (fabs(b) < WS_EPSILON) /* ray is parallel to triangle plane */
        return 0;

    /* Get intersection point of ray with triangle */
    r = a / b;
    if (r < 0.0) /* ray goes away from triangle -> no intersect */
        return 0;
    if (r > 1.0) /* ray is not long enough to reach triangle (comment out for ray-triangle) */
        return 0;

    /* Intersect point of ray with triangle */
    intersect = dir;
    vec3_mul_scalar(intersect.xyz, r);
    vec3_add_vec3(intersect.xyz, p1);

    /* Need to determine if intersect point is inside triangle */

    /* Cache dot products */
    w = intersect;
    vec3_sub_vec3(w.xyz, face->vertices[0].position.xyz);
    uu = vec3_dot(u.xyz, u.xyz);
    vv = vec3_dot(v.xyz, v.xyz);
    uv = vec3_dot(u.xyz, v.xyz);
    wu = vec3_dot(w.xyz, u.xyz);
    wv = vec3_dot(w.xyz, v.xyz);

    /* Calculate determinant */
    determinant = uv*uv - uu*vv;

    /* Calculate parametric coordinates and test */
    s = (uv*wv - vv*wu) / determinant;
    if (s < 0.0 || s > 1.0) /* intersect point is outside of face */
        return 0;
    t = (uv*wu - uu*wv) / determinant;
    if (t < 0.0 || s+t > 1.0)  /* intersect point is outside of face */
        return 0;

    result->count = 1;
    result->shape[0] = intersect;
    return 1;
}

/* ------------------------------------------------------------------------- */
int
intersect_face_aabb(intersect_result_t* result, const face_t* face, const WS_REAL aabb[6])
{
    intersect_result_t edge;
    result->count = 0;

    /*
     * If any of the face vertices are inside the bounding box, then we know
     * the face intersects.
     */
    if (intersect_point_aabb_test(face->vertices[0].position.xyz, aabb) ||
        intersect_point_aabb_test(face->vertices[0].position.xyz, aabb) ||
        intersect_point_aabb_test(face->vertices[0].position.xyz, aabb))
    {

    }

    /* These are the 8 vertices of the bounding box */
    vec3_t aaa = vec3(aabb[0], aabb[1], aabb[2]);
    vec3_t aab = vec3(aabb[0], aabb[1], aabb[5]);
    vec3_t aba = vec3(aabb[0], aabb[4], aabb[2]);
    vec3_t abb = vec3(aabb[0], aabb[4], aabb[5]);
    vec3_t baa = vec3(aabb[3], aabb[1], aabb[2]);
    vec3_t bab = vec3(aabb[3], aabb[1], aabb[5]);
    vec3_t bba = vec3(aabb[3], aabb[4], aabb[2]);
    vec3_t bbb = vec3(aabb[3], aabb[4], aabb[5]);

    /* Test all 12 edges of the bounding box for intersections with the face */
    if (intersect_line_face(&edge, aaa.xyz, aab.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aaa.xyz, aba.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aaa.xyz, baa.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aab.xyz, abb.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aab.xyz, bab.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aab.xyz, bba.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aba.xyz, abb.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aba.xyz, bab.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, aba.xyz, bba.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, baa.xyz, abb.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, baa.xyz, bab.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, baa.xyz, bba.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, abb.xyz, bbb.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, bab.xyz, bbb.xyz, face))
        result->shape[result->count++] = edge.shape[0];
    if (intersect_line_face(&edge, bba.xyz, bbb.xyz, face))
        result->shape[result->count++] = edge.shape[0];

    return 1;
}

/* ------------------------------------------------------------------------- */
int
intersect_face_aabb_test(const face_t* face, const WS_REAL aabb[6])
{
    intersect_result_t edge;

    /*
     * If any of the face vertices are inside the bounding box, then we know
     * the face intersects.
     */
    if (intersect_point_aabb_test(face->vertices[0].position.xyz, aabb) ||
        intersect_point_aabb_test(face->vertices[0].position.xyz, aabb) ||
        intersect_point_aabb_test(face->vertices[0].position.xyz, aabb))
    {
        return 1;
    }

    /* These are the 8 vertices of the bounding box */
    vec3_t aaa = vec3(aabb[0], aabb[1], aabb[2]);
    vec3_t aab = vec3(aabb[0], aabb[1], aabb[5]);
    vec3_t aba = vec3(aabb[0], aabb[4], aabb[2]);
    vec3_t abb = vec3(aabb[0], aabb[4], aabb[5]);
    vec3_t baa = vec3(aabb[3], aabb[1], aabb[2]);
    vec3_t bab = vec3(aabb[3], aabb[1], aabb[5]);
    vec3_t bba = vec3(aabb[3], aabb[4], aabb[2]);
    vec3_t bbb = vec3(aabb[3], aabb[4], aabb[5]);

    /* Test all 12 edges of the bounding box for intersections with the face */
    if (intersect_line_face(&edge, aaa.xyz, aab.xyz, face)) return 1;
    if (intersect_line_face(&edge, aaa.xyz, aba.xyz, face)) return 1;
    if (intersect_line_face(&edge, aaa.xyz, baa.xyz, face)) return 1;
    if (intersect_line_face(&edge, aab.xyz, abb.xyz, face)) return 1;
    if (intersect_line_face(&edge, aab.xyz, bab.xyz, face)) return 1;
    if (intersect_line_face(&edge, aab.xyz, bba.xyz, face)) return 1;
    if (intersect_line_face(&edge, aba.xyz, abb.xyz, face)) return 1;
    if (intersect_line_face(&edge, aba.xyz, bab.xyz, face)) return 1;
    if (intersect_line_face(&edge, aba.xyz, bba.xyz, face)) return 1;
    if (intersect_line_face(&edge, baa.xyz, abb.xyz, face)) return 1;
    if (intersect_line_face(&edge, baa.xyz, bab.xyz, face)) return 1;
    if (intersect_line_face(&edge, baa.xyz, bba.xyz, face)) return 1;
    if (intersect_line_face(&edge, abb.xyz, bbb.xyz, face)) return 1;
    if (intersect_line_face(&edge, bab.xyz, bbb.xyz, face)) return 1;
    if (intersect_line_face(&edge, bba.xyz, bbb.xyz, face)) return 1;

    return 0;
}

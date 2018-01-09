#include "wavesim/intersections.h"

/* ------------------------------------------------------------------------- */
intersect_result_t
intersect_line_face(vec3_t a, vec3_t b, const face_t* face)
{
    vec3_t u, v, n, dir, w0, intersect;
    real determinant, r, uu, vv, uv, wu, wv, s, t;
    intersect_result_t result = {0};

    /*
     * Algorithm taken from:
     * http://geomalgorithms.com/a06-_intersect-2.html
     */

    /* Get face edge vectors and plane normal */
    u = face->vertices[1].position;
    v = face->vertices[2].position;
    n = u;
    vec3_sub_vec3(u.f, face->vertices[0].position.f);
    vec3_sub_vec3(v.f, face->vertices[0].position.f);
    vec3_cross(n.f, v.f);

    /* Not checking for degenerate triangle, but could do so if n == 0 */

    /* Calculate ray direction vector */
    dir = b;
    w0 = a;
    vec3_sub_vec3(dir.f, a.f);
    vec3_sub_vec3(w0.f, face->vertices[0].position.f);

    /* Not checking for line being parallel to triangle */

    /* Get intersection point of ray with triangle */
    r = -vec3_dot(n.f, w0.f) / vec3_dot(n.f, dir.f);
    if (r < 0.0) /* ray goes away from triangle -> no intersect */
        return result;
    if (r > 1.0) /* ray is not long enough to reach triangle (comment out for ray-triangle) */

    /* Intersect point of ray with triangle */
    intersect = dir;
    vec3_mul_scalar(intersect.f, r);
    vec3_add_vec3(intersect.f, a.f);

    /* Need to determine if intersect point is inside triangle */

    /* Cache dot products */
    uu = vec3_dot(u.f, u.f);
    vv = vec3_dot(v.f, v.f);
    uv = vec3_dot(u.f, v.f);
    wu = vec3_dot(w0.f, u.f);
    wv = vec3_dot(w0.f, v.f);

    /* Calculate determinant */
    determinant = uv*uv - uu*vv;

    /* Now we can calculate s and t */
    s = (uv*wv - vv*wu) / determinant;
    if (s < 0.0 || s > 1.0) /* intersect point is outside of face */
        return result;
    t = (uv*wu - uu*wv) / determinant;
    if (t < 0.0 || t > 1.0)  /* intersect point is outside of face */
        return result;

    result.count = 1;
    result.shape[0] = intersect;
    return result;
}

/* ------------------------------------------------------------------------- */
intersect_result_t
intersect_aabb_face(aabb_t aabb, const face_t* face)
{
    intersect_result_t result = {0};
    intersect_result_t edge;

    /*
     * If any of the face vertices are inside the bounding box, then we know
     * the face intersects.
     */
    if (aabb_point_is_inside(&aabb, face->vertices[0].position) ||
        aabb_point_is_inside(&aabb, face->vertices[0].position) ||
        aabb_point_is_inside(&aabb, face->vertices[0].position))
    {

    }

    /* These are the 8 vertices of the bounding box */
    vec3_t aaa = aabb.a;
    vec3_t aab = vec3(aabb.a.v.x, aabb.a.v.y, aabb.b.v.z);
    vec3_t aba = vec3(aabb.a.v.x, aabb.b.v.y, aabb.a.v.z);
    vec3_t abb = vec3(aabb.a.v.x, aabb.b.v.y, aabb.b.v.z);
    vec3_t baa = vec3(aabb.b.v.x, aabb.a.v.y, aabb.a.v.z);
    vec3_t bab = vec3(aabb.b.v.x, aabb.a.v.y, aabb.b.v.z);
    vec3_t bba = vec3(aabb.b.v.x, aabb.b.v.y, aabb.a.v.z);
    vec3_t bbb = aabb.b;

    /* Test all 12 edges of the bounding box for intersections with the face */
    if ((edge = intersect_line_face(aaa, aab, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aaa, aba, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aaa, baa, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aab, abb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aab, bab, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aab, bba, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aba, abb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aba, bab, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(aba, bba, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(baa, abb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(baa, bab, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(baa, bba, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(abb, bbb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(bab, bbb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];
    if ((edge = intersect_line_face(bba, bbb, face)).count > 0)
        result.shape[result.count++] = edge.shape[0];

    return result;
}

/* ------------------------------------------------------------------------- */
int
intersect_aabb_face_test(aabb_t aabb, const face_t* face)
{
    intersect_result_t edge;

    /*
     * If any of the face vertices are inside the bounding box, then we know
     * the face intersects.
     */
    if (aabb_point_is_inside(&aabb, face->vertices[0].position) ||
        aabb_point_is_inside(&aabb, face->vertices[0].position) ||
        aabb_point_is_inside(&aabb, face->vertices[0].position))
    {
        return 1;
    }

    /* These are the 8 vertices of the bounding box */
    vec3_t aaa = aabb.a;
    vec3_t aab = vec3(aabb.a.v.x, aabb.a.v.y, aabb.b.v.z);
    vec3_t aba = vec3(aabb.a.v.x, aabb.b.v.y, aabb.a.v.z);
    vec3_t abb = vec3(aabb.a.v.x, aabb.b.v.y, aabb.b.v.z);
    vec3_t baa = vec3(aabb.b.v.x, aabb.a.v.y, aabb.a.v.z);
    vec3_t bab = vec3(aabb.b.v.x, aabb.a.v.y, aabb.b.v.z);
    vec3_t bba = vec3(aabb.b.v.x, aabb.b.v.y, aabb.a.v.z);
    vec3_t bbb = aabb.b;

    /* Test all 12 edges of the bounding box for intersections with the face */
    if ((edge = intersect_line_face(aaa, aab, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aaa, aba, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aaa, baa, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aab, abb, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aab, bab, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aab, bba, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aba, abb, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aba, bab, face)).count > 0) return 1;
    if ((edge = intersect_line_face(aba, bba, face)).count > 0) return 1;
    if ((edge = intersect_line_face(baa, abb, face)).count > 0) return 1;
    if ((edge = intersect_line_face(baa, bab, face)).count > 0) return 1;
    if ((edge = intersect_line_face(baa, bba, face)).count > 0) return 1;
    if ((edge = intersect_line_face(abb, bbb, face)).count > 0) return 1;
    if ((edge = intersect_line_face(bab, bbb, face)).count > 0) return 1;
    if ((edge = intersect_line_face(bba, bbb, face)).count > 0) return 1;

    return 0;
}

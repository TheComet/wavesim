#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef struct face_t face_t;

typedef struct intersect_result_t
{
    int count;           /* The total number of intersections */
    vec3_t shape[6];     /* 3D points defining the shape of the intersection */
    vec3_t bary[6];      /* Barycentric coordinates of the 3D points in "shape" */
} intersect_result_t;

WAVESIM_PRIVATE_API int
intersect_point_aabb_test(const wsreal_t point[3], const wsreal_t aabb[6]);

WAVESIM_PRIVATE_API int
intersect_aabb_aabb_test(const wsreal_t aabb1[6], const wsreal_t aabb2[6]);

WAVESIM_PRIVATE_API int
intersect_line_face(intersect_result_t* result,
                    const wsreal_t p1[3], const wsreal_t p2[3],
                    const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3]);

WAVESIM_PRIVATE_API int
intersect_line_face_test(const wsreal_t p1[3], const wsreal_t p2[3],
                         const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3]);

WAVESIM_PRIVATE_API int
intersect_face_aabb(intersect_result_t* result,
                    const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3],
                    const wsreal_t aabb[6]);

/*!
 * @brief Faster test, but doesn't return any points of intersection, just that
 * it did intersect.
 * @return Returns 1 if an intersection occurred, 0 if no intersection occurred.
 */
WAVESIM_PRIVATE_API int
intersect_face_aabb_test(const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3],
                         const wsreal_t aabb[6]);

C_END

#endif /* INTERSECTIONS_H */

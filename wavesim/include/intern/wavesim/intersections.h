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
} intersect_result_t;

WAVESIM_PRIVATE_API int
intersect_point_aabb_test(const WS_REAL point[3], const WS_REAL aabb[6]);

WAVESIM_PRIVATE_API int
intersect_aabb_aabb_test(const WS_REAL aabb1[6], const WS_REAL aabb2[6]);

WAVESIM_PRIVATE_API intersect_result_t
intersect_line_face(const WS_REAL a[3], const WS_REAL b[3], const face_t* face);

WAVESIM_PRIVATE_API intersect_result_t
intersect_face_aabb(const face_t* face, const WS_REAL aabb[6]);

/*!
 * @brief Faster test, but doesn't return any points of intersection, just that
 * it did intersect.
 * @return Returns 1 if an intersection occurred, 0 if no intersection occurred.
 */
WAVESIM_PRIVATE_API int
intersect_face_aabb_test(const face_t* face, const WS_REAL aabb[6]);

C_END

#endif /* INTERSECTIONS_H */

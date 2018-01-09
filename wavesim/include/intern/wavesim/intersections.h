#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "wavesim/config.h"
#include "wavesim/aabb.h"
#include "wavesim/face.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef struct intersect_result_t
{
    int count;           /* The total number of intersections */
    vec3_t shape[6];     /* 3D points defining the shape of the intersection */
} intersect_result_t;

WAVESIM_PRIVATE_API intersect_result_t
intersect_line_face(vec3_t a, vec3_t b, const face_t* face);

WAVESIM_PRIVATE_API intersect_result_t
intersect_aabb_face(aabb_t aabb, const face_t* face);

/*!
 * @brief Faster test, but doesn't return any points of intersection, just that
 * it did intersect.
 * @return Returns 1 if an intersection occurred, 0 if no intersection occurred.
 */
WAVESIM_PRIVATE_API int
intersect_aabb_face_test(aabb_t aabb, const face_t* face);

C_END

#endif /* INTERSECTIONS_H */

#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

/*!
 * @brief Tests if a point lies within an axis-aligned bounding-box
 * @param[in] point x,y,z coordinates of the point.
 * @param[in] aabb x1,y1,z1,x2,y2,z2 coordinates of the two diagonal points of
 * a bounding box.
 * @return Returns non-zero ("true") if the point lies within the bounding box.
 * 0 if otherwise.
 */
WAVESIM_PRIVATE_API int
intersect_point_aabb_test(const wsreal_t point[3], const wsreal_t aabb[6]);

/*!
 * @brief Tests if two axis-aligned bounding-boxes intersect or not.
 * @param[in] aabb1 x1,y1,z1,x2,y2,z2 coordinates of the two diagonal points of
 * the first bounding box.
 * @param[in] aabb2 x1,y1,z1,x2,y2,z2 coordinates of the two diagonal points of
 * the second bounding box.
 * @return Returns non-zero ("true") if the AABBs intersect. 0 if otherwise.
 */
WAVESIM_PRIVATE_API int
intersect_aabb_aabb_test(const wsreal_t aabb1[6], const wsreal_t aabb2[6]);

/*!
 * @brief Calculates the intersection point of a line and a plane.
 * @param[out] result x,y,z coordinates of the point of intersection.
 * @param[in] p1/p2 x,y,z coordinates of the beginning and end of the line
 * segment.
 * @param[in] v1/v2/v3 x,y,z coordinates of 3 points that lie within the plane.
 * @return Returns non-zero ("true") if the point of intersection is defined.
 * Returns 0 if the line is parallel to the plane or if the line doesn't
 * intersect the plane, in which case, nothing is written to result.
 */
WAVESIM_PRIVATE_API int
intersect_line_plane(wsreal_t result[3],
                     const wsreal_t p1[3], const wsreal_t p2[3],
                     const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3]);

WAVESIM_PRIVATE_API int
intersect_plane_aabb_test(const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3],
                          const wsreal_t aabb[6]);

/*!
 * @brief Calculates the intersection point of a line segment with a triangle.
 * @param[out] result The barycentric coordinates s,t,w of the intersection are
 * written to this parameter.
 * @param[in] p1/p2 x,y,z coordinates of the beginning and end of the line
 * segment.
 * @param[in] v1/v2/v3 x,y,z coordinates of the 3 vertices that define the
 * triangle.
 * @return Returns non-zero ("true") if the point of intersection lies within
 * the triangle. Returns 0 if there was no intersection.
 */
WAVESIM_PRIVATE_API int
intersect_line_triangle_barycentric(wsreal_t result[3],
                                    const wsreal_t p1[3], const wsreal_t p2[3],
                                    const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3]);

/*!
 * @brief Calculates the intersection point of a line segment with a triangle.
 * @param[out] result The cartesian coordinates x,y,z of the intersection are
 * written to this parameter.
 * @param[in] p1/p2 x,y,z coordinates of the beginning and end of the line
 * segment.
 * @param[in] v1/v2/v3 x,y,z coordinates of the 3 vertices that define the
 * triangle.
 * @return Returns non-zero ("true") if the point of intersection lies within
 * the triangle. Returns 0 if there was no intersection.
 */
WAVESIM_PRIVATE_API int
intersect_line_triangle_cartesian(wsreal_t result[3],
                                  const wsreal_t p1[3], const wsreal_t p2[3],
                                  const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3]);

WAVESIM_PRIVATE_API int
intersect_line_triangle_test(const wsreal_t p0[3], const wsreal_t p1[3],
                             const wsreal_t v0[3], const wsreal_t v1[3], const wsreal_t v2[3]);

/*!
 * @brief Tests whether an axis-aligned bounding-box intersects a triangle.
 */
WAVESIM_PRIVATE_API int
intersect_triangle_aabb_test(const wsreal_t v1[3], const wsreal_t v2[3], const wsreal_t v3[3],
                             const wsreal_t aabb[6]);

C_END

#endif /* INTERSECTIONS_H */

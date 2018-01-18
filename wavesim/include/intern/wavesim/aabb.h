#ifndef AABB_H
#define AABB_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef union aabb_t
{
    struct {
        vec3_t min;
        vec3_t max;
    } b;
    WS_REAL xyzxyz[6];
} aabb_t;

#define AABB_AX(X) X.b.min.v.x
#define AABB_AY(X) X.b.min.v.y
#define AABB_AZ(X) X.b.min.v.z
#define AABB_BX(X) X.b.max.v.x
#define AABB_BY(X) X.b.max.v.y
#define AABB_BZ(X) X.b.max.v.z

#define AABB_DIMS(X) \
    vec3(AABB_BX(X) - AABB_AX(X), \
         AABB_BY(X) - AABB_AY(X), \
         AABB_BZ(X) - AABB_AZ(X))

/*!
 * @brief Creates a new axis-aligned bounding-box with the specified extents.
 * @param[in] ax+ay+az 3D coordinates of the minimum extent (left bottom front)
 * @param[in] bx+by+bz 3D coordinates of the maximum extent (right top back)
 * @return Returns the bounding box.
 */
WAVESIM_PRIVATE_API aabb_t
aabb(WS_REAL ax, WS_REAL ay, WS_REAL az, WS_REAL bx, WS_REAL by, WS_REAL bz);

/*!
 * @brief Creates a new "reset" axis-aligned bounding-box. "Reset" means that
 * the extents are set to infinity and conflict.
 */
WAVESIM_PRIVATE_API aabb_t
aabb_reset(void);

/*!
 * @brief Calculates the axis-aligned bounding-box of 3 points.
 */
WAVESIM_PRIVATE_API aabb_t
aabb_from_3_points(const WS_REAL p1[3], const WS_REAL p2[3], const WS_REAL p3[3]);

/*!
 * @brief Expands an existing axis-aligned bounding-box (if necessary) to
 * include an additional 3D point in space.
 */
WAVESIM_PRIVATE_API void
aabb_expand_point(WS_REAL aabb[6], const WS_REAL p[3]);

C_END

#endif /* AABB_H */

#include "wavesim/aabb.h"
#include "wavesim/face.h"
#include "math.h"

/* ------------------------------------------------------------------------- */
aabb_t
aabb(real ax, real ay, real az, real bx, real by, real bz)
{
    return (aabb_t){
        {{ax, ay, az}},
        {{bx, by, bz}},
    };
}

aabb_t
aabb_reset(void)
{
    return aabb(-INFINITY, -INFINITY, -INFINITY, INFINITY, INFINITY, INFINITY);
}

/* ------------------------------------------------------------------------- */
int
aabb_point_is_inside(const aabb_t* aabb, vec3_t point)
{
    return (
        point.v.x >= aabb->a.v.x && point.v.x <= aabb->b.v.x &&
        point.v.y >= aabb->a.v.y && point.v.y <= aabb->b.v.y &&
        point.v.z >= aabb->a.v.z && point.v.z <= aabb->b.v.z
    );
}

/* ------------------------------------------------------------------------- */
int
aabb_face_is_inside(const aabb_t* aabb, const face_t* face)
{
    return (
        aabb_point_is_inside(aabb, face->vertices[0].position) &&
        aabb_point_is_inside(aabb, face->vertices[1].position) &&
        aabb_point_is_inside(aabb, face->vertices[2].position)
    );
}

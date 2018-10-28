#include "gmock/gmock.h"
#include "wavesim/aabb.h"

#define NAME aabb

using namespace ::testing;

TEST(NAME, reset_is_correct)
{
    aabb_t bb = aabb_reset();
    EXPECT_THAT(AABB_AX(bb), DoubleEq(std::numeric_limits<wsreal_t>::infinity()));
    EXPECT_THAT(AABB_AY(bb), DoubleEq(std::numeric_limits<wsreal_t>::infinity()));
    EXPECT_THAT(AABB_AZ(bb), DoubleEq(std::numeric_limits<wsreal_t>::infinity()));
    EXPECT_THAT(AABB_BX(bb), DoubleEq(-std::numeric_limits<wsreal_t>::infinity()));
    EXPECT_THAT(AABB_BY(bb), DoubleEq(-std::numeric_limits<wsreal_t>::infinity()));
    EXPECT_THAT(AABB_BZ(bb), DoubleEq(-std::numeric_limits<wsreal_t>::infinity()));
}

TEST(NAME, constructor_assigns_values_correctly)
{
    aabb_t bb = aabb(1, 2, 3, 4, 5, 6);
    EXPECT_THAT(bb.b.min.v.x, DoubleEq(1.0));
    EXPECT_THAT(bb.b.min.v.y, DoubleEq(2.0));
    EXPECT_THAT(bb.b.min.v.z, DoubleEq(3.0));
    EXPECT_THAT(bb.b.max.v.x, DoubleEq(4.0));
    EXPECT_THAT(bb.b.max.v.y, DoubleEq(5.0));
    EXPECT_THAT(bb.b.max.v.z, DoubleEq(6.0));
}

TEST(NAME, macros_retrieve_correct_values)
{
    aabb_t bb = aabb(1, 2, 3, 4, 5, 6);
    EXPECT_THAT(AABB_AX(bb), DoubleEq(1.0));
    EXPECT_THAT(AABB_AY(bb), DoubleEq(2.0));
    EXPECT_THAT(AABB_AZ(bb), DoubleEq(3.0));
    EXPECT_THAT(AABB_BX(bb), DoubleEq(4.0));
    EXPECT_THAT(AABB_BY(bb), DoubleEq(5.0));
    EXPECT_THAT(AABB_BZ(bb), DoubleEq(6.0));
}

TEST(NAME, dimensions_are_calculated_correctly)
{
    aabb_t bb = aabb(1, 2, 3, 5, 7, 9);
    vec3_t dims = AABB_DIMS(bb);
    EXPECT_THAT(dims.v.x, DoubleEq(4.0)); // 5-1
    EXPECT_THAT(dims.v.y, DoubleEq(5.0)); // 7-2
    EXPECT_THAT(dims.v.z, DoubleEq(6.0)); // 9-3
}

TEST(NAME, from_3_points_1)
{
    vec3_t a = vec3(-5, -2, -3);
    vec3_t b = vec3(1, 1, 1);
    vec3_t c = vec3(6, 2, 6);
    aabb_t bb = aabb_from_3_points(a.xyz, b.xyz, c.xyz);

    EXPECT_THAT(bb.b.min.v.x, DoubleEq(a.v.x));
    EXPECT_THAT(bb.b.min.v.y, DoubleEq(a.v.y));
    EXPECT_THAT(bb.b.min.v.z, DoubleEq(a.v.z));
    EXPECT_THAT(bb.b.max.v.x, DoubleEq(c.v.x));
    EXPECT_THAT(bb.b.max.v.y, DoubleEq(c.v.y));
    EXPECT_THAT(bb.b.max.v.z, DoubleEq(c.v.z));
}

TEST(NAME, from_3_points_2)
{
    vec3_t a = vec3(-5, 2, 6);
    vec3_t b = vec3(1, -2, 1);
    vec3_t c = vec3(6, 1, -3);
    aabb_t bb = aabb_from_3_points(a.xyz, b.xyz, c.xyz);

    EXPECT_THAT(bb.b.min.v.x, DoubleEq(-5));
    EXPECT_THAT(bb.b.min.v.y, DoubleEq(-2));
    EXPECT_THAT(bb.b.min.v.z, DoubleEq(-3));
    EXPECT_THAT(bb.b.max.v.x, DoubleEq(6));
    EXPECT_THAT(bb.b.max.v.y, DoubleEq(2));
    EXPECT_THAT(bb.b.max.v.z, DoubleEq(6));
}

TEST(NAME, expand_point)
{
    aabb_t bb = aabb_reset();
    vec3_t p = vec3(2, 3, 4);
    aabb_expand_point(bb.xyzxyz, p.xyz);
    p = vec3(5, 5, 5);
    aabb_expand_point(bb.xyzxyz, p.xyz);
    p = vec3(-6, 3, 2);
    aabb_expand_point(bb.xyzxyz, p.xyz);
    p = vec3(5, -8, 5);
    aabb_expand_point(bb.xyzxyz, p.xyz);
    p = vec3(1, 1, 3);
    aabb_expand_point(bb.xyzxyz, p.xyz);

    EXPECT_THAT(bb.b.min.v.x, DoubleEq(-6));
    EXPECT_THAT(bb.b.min.v.y, DoubleEq(-8));
    EXPECT_THAT(bb.b.min.v.z, DoubleEq(2));
    EXPECT_THAT(bb.b.max.v.x, DoubleEq(5));
    EXPECT_THAT(bb.b.max.v.y, DoubleEq(5));
    EXPECT_THAT(bb.b.max.v.z, DoubleEq(5));
}

#include "gmock/gmock.h"
#include "wavesim/vec3.h"

#define NAME vec3

using namespace ::testing;

TEST(NAME, cross_product_1)
{
    vec3_t a = vec3(0, 0, 2);
    vec3_t b = vec3(0, 2, 0);
    vec3_cross(a.xyz, b.xyz);
    EXPECT_THAT(a.v.x, DoubleEq(-4));
    EXPECT_THAT(a.v.y, DoubleEq(0));
    EXPECT_THAT(a.v.z, DoubleEq(0));
}

TEST(NAME, cross_product_2)
{
    vec3_t a = vec3(0, 0, 2);
    vec3_t b = vec3(2, 0, 0);
    vec3_cross(a.xyz, b.xyz);
    EXPECT_THAT(a.v.x, DoubleEq(0));
    EXPECT_THAT(a.v.y, DoubleEq(4));
    EXPECT_THAT(a.v.z, DoubleEq(0));
}

TEST(NAME, cross_product_3)
{
    vec3_t a = vec3(0, 2, 0);
    vec3_t b = vec3(2, 0, 0);
    vec3_cross(a.xyz, b.xyz);
    EXPECT_THAT(a.v.x, DoubleEq(0));
    EXPECT_THAT(a.v.y, DoubleEq(0));
    EXPECT_THAT(a.v.z, DoubleEq(-4));
}

TEST(NAME, cross_product_4)
{
    vec3_t a = vec3(1, 2, 3);
    vec3_t b = vec3(4, 5, 6);
    vec3_cross(a.xyz, b.xyz);
    EXPECT_THAT(a.v.x, DoubleEq(-3));
    EXPECT_THAT(a.v.y, DoubleEq(6));
    EXPECT_THAT(a.v.z, DoubleEq(-3));
}

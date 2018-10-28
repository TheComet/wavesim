#include "gmock/gmock.h"
#include "wavesim/mesh/vertex.h"

#define NAME vertex

using namespace ::testing;

TEST(NAME, constructs_correctly)
{
    vertex_t v = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(0, 0, 0)));
    EXPECT_THAT(v.position.v.x, DoubleEq(1));
    EXPECT_THAT(v.position.v.y, DoubleEq(2));
    EXPECT_THAT(v.position.v.z, DoubleEq(3));
    EXPECT_THAT(v.attr.reflection, DoubleEq(4));
    EXPECT_THAT(v.attr.transmission, DoubleEq(5));
    EXPECT_THAT(v.attr.absorption, DoubleEq(6));
    EXPECT_THAT(v.attr.sound_velocity, DoubleEq(340));
    EXPECT_THAT(v.attr.velocity.v.x, DoubleEq(0));
    EXPECT_THAT(v.attr.velocity.v.y, DoubleEq(0));
    EXPECT_THAT(v.attr.velocity.v.z, DoubleEq(0));
}

TEST(NAME, equality_checks)
{
    vertex_t v1 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(0, 0, 0)));
    vertex_t v2 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(0, 0, 0)));
    vertex_t v3 = vertex(vec3(2, 2, 3), attribute(4, 5, 6, 340, vec3(0, 0, 0)));
    vertex_t v4 = vertex(vec3(1, 2, 3), attribute(1, 5, 6, 340, vec3(0, 0, 0)));
    vertex_t v5 = vertex(vec3(1, 2, 3), attribute(4, 6, 6, 340, vec3(0, 0, 0)));
    vertex_t v6 = vertex(vec3(1, 2, 3), attribute(4, 5, 7, 340, vec3(0, 0, 0)));
    vertex_t v7 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 341, vec3(0, 0, 0)));
    vertex_t v8 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(1, 0, 0)));
    vertex_t v9 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(0, 1, 0)));
    vertex_t v10 = vertex(vec3(1, 2, 3), attribute(4, 5, 6, 340, vec3(0, 0, 1)));
    EXPECT_THAT(vertex_is_same(&v1, &v2), Eq(true));
    EXPECT_THAT(vertex_is_same(&v1, &v3), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v4), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v5), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v6), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v7), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v8), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v9), Eq(false));
    EXPECT_THAT(vertex_is_same(&v1, &v10), Eq(false));
}

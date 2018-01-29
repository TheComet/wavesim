#include "gmock/gmock.h"
#include "wavesim/intersections.h"
#include "wavesim/aabb.h"
#include "wavesim/face.h"

#define NAME intersections

using namespace testing;

TEST(NAME, point_in_aabb)
{
    aabb_t bb = aabb(3, 1, 6, 4, 2, 7);
    wsreal_t p1[3] = {3, 1, 6};
    wsreal_t p2[3] = {4, 2, 7};
    wsreal_t p3[3] = {3.5, 1.5, 6.5};
    EXPECT_THAT(intersect_point_aabb_test(p1, bb.xyzxyz), Eq(true));
    EXPECT_THAT(intersect_point_aabb_test(p2, bb.xyzxyz), Eq(true));
    EXPECT_THAT(intersect_point_aabb_test(p3, bb.xyzxyz), Eq(true));
}

TEST(NAME, point_outside_aabb)
{
    aabb_t bb = aabb(3, 1, 6, 4, 2, 7);
    wsreal_t points[6][3] = {
        {2.9, 1, 6},   // Test X axis
        {4.1, 2, 7},   // Test X axis
        {2.9, 0.9, 6}, // Test Y axis
        {4.1, 2.1, 7}, // Test Y axis
        {2.9, 1, 5.9}, // Test Z axis
        {4.1, 2, 7.1}  // Test Z axis
    };

    for (int i = 0; i != 6; ++i)
        EXPECT_THAT(intersect_point_aabb_test(points[i], bb.xyzxyz), Eq(false));
}

TEST(NAME, line_face_intersects_1)
{
    // This line will intersect the face at 0, 0, 0
    wsreal_t p1[3] = { 1,  1,  1};
    wsreal_t p2[3] = {-1, -1, -1};
    wsreal_t v1[3] = {-1,  0,  1};
    wsreal_t v2[3] = { 1,  0,  1};
    wsreal_t v3[3] = { 0,  0, -1};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(1));
    EXPECT_THAT(result.v.x, DoubleEq(0.0));
    EXPECT_THAT(result.v.y, DoubleEq(0.0));
    EXPECT_THAT(result.v.z, DoubleEq(0.0));
}

TEST(NAME, line_face_intersects_2)
{
    // This misses the triangle
    wsreal_t p1[3] = { 1,  1,  1};
    wsreal_t p2[3] = {-1, -1, -1};
    wsreal_t v1[3] = {-1 , 1,  0};
    wsreal_t v2[3] = { 1 , 1,  0};
    wsreal_t v3[3] = { 0, -1,  0};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(1));
    EXPECT_THAT(result.v.x, DoubleEq(0.0));
    EXPECT_THAT(result.v.y, DoubleEq(0.0));
    EXPECT_THAT(result.v.z, DoubleEq(0.0));
}

TEST(NAME, line_face_intersects_close)
{
    // This misses the triangle
    wsreal_t p1[3] = { 1, 1, 1};
    wsreal_t p2[3] = {-0.01, -1, -1};
    wsreal_t v1[3] = {-1,  1,  0};
    wsreal_t v2[3] = { 1,  1,  0};
    wsreal_t v3[3] = { 0, -1,  0};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(1));
}

TEST(NAME, line_face_misses_1)
{
    // This misses the triangle
    wsreal_t p1[3] = { 1,  1,  1};
    wsreal_t p2[3] = { 1, -1, -1};
    wsreal_t v1[3] = {-1,  0,  1};
    wsreal_t v2[3] = { 1,  0,  1};
    wsreal_t v3[3] = { 0,  0,  1};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(0));
}

TEST(NAME, line_face_misses_2)
{
    // This misses the triangle
    wsreal_t p1[3] = { 1,  1,  1};
    wsreal_t p2[3] = { 1, -1, -1};
    wsreal_t v1[3] = {-1,  1,  0};
    wsreal_t v2[3] = { 1,  1,  0};
    wsreal_t v3[3] = { 0, -1,  0};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(0));
}

TEST(NAME, line_face_misses_close)
{
    // This misses the triangle
    wsreal_t p1[3] = {1, 1, 1};
    wsreal_t p2[3] = {0.01, -1, -1};
    wsreal_t v1[3] = {-1,  1,  0};
    wsreal_t v2[3] = { 1,  1,  0};
    wsreal_t v3[3] = { 0, -1,  0};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(0));
}

TEST(NAME, line_face_segment_too_short)
{
    // Line ends before hitting the face
    wsreal_t p1[3] = {3, 3, 3};
    wsreal_t p2[3] = {0.01, 0.01, 0.01};
    wsreal_t v1[3] = {-1,  0,  1};
    wsreal_t v2[3] = { 1,  0,  1};
    wsreal_t v3[3] = { 0,  0, -1};
    vec3_t result;
    ASSERT_THAT(intersect_line_triangle_cartesian(result.xyz, p1, p2, v1, v2, v3), Eq(0));
}

TEST(NAME, face_aabb__face_intersects_aabb_edge)
{
    wsreal_t v1[3] = {-0.5, 1.5, 1.0};
    wsreal_t v2[3] = {-0.5, -0.5, 1.0};
    wsreal_t v3[3] = {1.5, -0.5, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb__face_inside_aabb)
{
    wsreal_t v1[3] = {0.6, 0.8, 1.0};
    wsreal_t v2[3] = {0.6, 0.6, 1.0};
    wsreal_t v3[3] = {0.8, 0.6, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb__face_edge_intersects_aabb_face)
{
    wsreal_t v1[3] = {0.5, 0.5, 1.0};
    wsreal_t v2[3] = {0.5, -1.5, 1.0};
    wsreal_t v3[3] = {2.5, -1.5, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_random_test_1)
{
    wsreal_t v1[3] = {-0.548506, 2.24598, 3.53052};
    wsreal_t v2[3] = {1.80669, 0.867556, 1.38982};
    wsreal_t v3[3] = {2.34748, 1.12558, 4.18314};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_random_test_2)
{
    wsreal_t v1[3] = {-0.548506, 2.24598, 3.53052};
    wsreal_t v2[3] = {2.58825, 1.30598, 2.83644};
    wsreal_t v3[3] = {1.54976, -1.20857, 1.32892};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, line_face_parallel_to_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
}

TEST(NAME, line_face_lines_inside_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
}

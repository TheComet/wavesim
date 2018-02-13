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

TEST(NAME, adjacent_aabbs_dont_intersect)
{
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(1, 0, 0, 2, 1, 1).xyzxyz), Eq(false));
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(0, 1, 0, 1, 2, 1).xyzxyz), Eq(false));
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(0, 0, 1, 1, 1, 2).xyzxyz), Eq(false));
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(-1, 0, 0, 0, 1, 1).xyzxyz), Eq(false));
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(0, -1, 0, 1, 0, 1).xyzxyz), Eq(false));
    EXPECT_THAT(intersect_aabb_aabb_test(aabb(0, 0, 0, 1, 1, 1).xyzxyz, aabb(0, 0, -1, 1, 1, 0).xyzxyz), Eq(false));
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

TEST(NAME, line_face_parallel_to_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
}

TEST(NAME, line_face_lines_inside_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
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

TEST(NAME, face_aabb__face_edge_intersects_aabb_face_1)
{
    wsreal_t v1[3] = {0.5, 0.5, 1.0};
    wsreal_t v2[3] = {0.5, -1.5, 1.0};
    wsreal_t v3[3] = {2.5, -1.5, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb__face_edge_misses_aabb_face_1)
{
    wsreal_t v1[3] = {0.5, -0.1, 1.0};
    wsreal_t v2[3] = {0.5, -2.1, 1.0};
    wsreal_t v3[3] = {2.5, -2.1, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb__face_edge_intersects_aabb_face_2)
{
    wsreal_t v1[3] = {2.5, 1.166667, 3.166667};
    wsreal_t v2[3] = {0.5, 1.166667, 3.166667};
    wsreal_t v3[3] = {0.5, 1.166667, 1.166667};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb__face_edge_misses_aabb_face_2)
{
    wsreal_t v1[3] = {2.5, 1.166667, 4.166667};
    wsreal_t v2[3] = {0.5, 1.166667, 4.166667};
    wsreal_t v3[3] = {0.5, 1.166667, 2.166667};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb__face_edge_intersects_aabb_face_3)
{
    wsreal_t v1[3] = {-1.5, 2.9, 1.0};
    wsreal_t v2[3] = {-1.5, 0.9, 1.0};
    wsreal_t v3[3] = {0.5, 0.9, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb__face_edge_misses_aabb_face_3)
{
    wsreal_t v1[3] = {-2.1, 2.9, 1.0};
    wsreal_t v2[3] = {-2.1, 0.9, 1.0};
    wsreal_t v3[3] = {-0.1, 0.9, 1.0};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
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

// ----------------------------------------------------------------------------
// Here we place a small triangle at each of the 8 corners of the AABB and move
// it slightly in range and out of range and test for collision. The numbers
// used were generated using blender.
// ----------------------------------------------------------------------------

TEST(NAME, face_aabb_corner_1_hit)
{
    wsreal_t v1[3] = {2.06742, 2.04637, 0.064936};
    wsreal_t v2[3] = {2.14564, 1.73473, -0.068178};
    wsreal_t v3[3] = {1.858, 2.0414, -0.008601};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_1_miss)
{
    wsreal_t v1[3] = {2.06742, 2.04637, 0.064936};
    wsreal_t v2[3] = {2.14564, 1.73473, -0.068178};
    wsreal_t v3[3] = {1.858, 2.0414, -0.063498};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_2_hit)
{
    wsreal_t v1[3] = {0.243852, 2.05632, -0.068178};
    wsreal_t v2[3] = {-0.113127, 1.83417, -0.008601};
    wsreal_t v3[3] = {-0.077066, 2.04052, 0.064936};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_2_miss)
{
    wsreal_t v1[3] = {0.243852, 2.10969, -0.068178};
    wsreal_t v2[3] = {-0.113127, 1.97005, -0.008601};
    wsreal_t v3[3] = {-0.077066, 2.04052, 0.064936};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_3_hit)
{
    wsreal_t v1[3] = {-0.113285, 0.246623, -0.05575};
    wsreal_t v2[3] = {-0.004857, -0.063243, 0.077364};
    wsreal_t v3[3] = {0.069539, -0.090289, 0.003827};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_3_miss)
{
    wsreal_t v1[3] = {-0.113285, 0.246623, -0.05575};
    wsreal_t v2[3] = {-0.004857, -0.063243, 0.077364};
    wsreal_t v3[3] = {0.04498, -0.090289, 0.003827};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_4_hit)
{
    wsreal_t v1[3] = {1.77072, -0.138888, -0.040964};
    wsreal_t v2[3] = {2.07856, 0.075068, -0.00144};
    wsreal_t v3[3] = {2.0801, 0.00305, 0.05716};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_4_miss)
{
    wsreal_t v1[3] = {1.77072, -0.138888, -0.040964};
    wsreal_t v2[3] = {2.07856, 0.060398, -0.00144};
    wsreal_t v3[3] = {2.0801, 0.00305, 0.05716};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_5_hit)
{
    wsreal_t v1[3] = {1.83598, -0.160644, 1.95757};
    wsreal_t v2[3] = {2.05632, 0.104544, 2.03891};
    wsreal_t v3[3] = {2.1156, 0.090686, 1.98892};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_5_miss)
{
    wsreal_t v1[3] = {1.83598, -0.160644, 1.95757};
    wsreal_t v2[3] = {2.05632, 0.104544, 2.07793};
    wsreal_t v3[3] = {2.1156, 0.073493, 1.98892};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_6_hit)
{
    wsreal_t v1[3] = {2.15026, 1.80039, 1.9392};
    wsreal_t v2[3] = {1.91743, 2.05469, 2.05955};
    wsreal_t v3[3] = {1.95193, 2.10979 ,1.97055};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_6_miss)
{
    wsreal_t v1[3] = {2.15026, 1.80039, 1.9392};
    wsreal_t v2[3] = {1.91743, 2.05469, 2.05955};
    wsreal_t v3[3] = {1.99092, 2.10979 ,1.97055};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_7_hit)
{
    wsreal_t v1[3] = {-0.031554, 1.90762, 2.05752};
    wsreal_t v2[3] = {0.198773, 2.16528, 1.93951};
    wsreal_t v3[3] = {-0.094749, 1.978456, 1.97181};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_7_miss)
{
    wsreal_t v1[3] = {-0.031554, 1.929785, 2.05752};
    wsreal_t v2[3] = {0.198773, 2.16528, 1.93951};
    wsreal_t v3[3] = {-0.094749, 1.978456, 1.97181};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

TEST(NAME, face_aabb_corner_8_hit)
{
    wsreal_t v1[3] = {-0.18047, 0.166927, 1.937108};
    wsreal_t v2[3] = {0.084902, -0.02823, 2.05512};
    wsreal_t v3[3] = {0.04554, -0.097607, 1.969404};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, face_aabb_corner_8_miss)
{
    wsreal_t v1[3] = {-0.18047, 0.166927, 1.937108};
    wsreal_t v2[3] = {0.063209, -0.02823, 2.05512};
    wsreal_t v3[3] = {0.04554, -0.097607, 1.969404};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(0));
}

// ----------------------------------------------------------------------------
// Edge cases
// ----------------------------------------------------------------------------

TEST(NAME, smaller_face_lies_within_aabb_face)
{
    wsreal_t v1[3] = {0, 1.9, 1.9};
    wsreal_t v2[3] = {0, 1.9, 0.1};
    wsreal_t v3[3] = {0, 0.1, 0.1};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, larger_face_lies_within_aabb_face)
{
    wsreal_t v1[3] = {0, 2.1, 2.1};
    wsreal_t v2[3] = {0, 2.1, -0.1};
    wsreal_t v3[3] = {0, -0.1, -0.1};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, aabb_corner_touches_edge_of_face)
{
    wsreal_t v1[3] = {0, 3, 1};
    wsreal_t v2[3] = {0, 1, -1};
    wsreal_t v3[3] = {0, 3, -1};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

TEST(NAME, aabb_edge_touches_edge_of_face)
{
    wsreal_t v1[3] = {1, 3, 1};
    wsreal_t v2[3] = {1, 1, -1};
    wsreal_t v3[3] = {1, 3, -1};
    wsreal_t bb[6] = {0, 0, 0, 2, 2, 2};
    ASSERT_THAT(intersect_triangle_aabb_test(v1, v2, v3, bb), Eq(1));
}

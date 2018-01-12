#include "gmock/gmock.h"
#include "wavesim/intersections.h"
#include "wavesim/aabb.h"
#include "wavesim/face.h"

#define NAME intersections

using namespace testing;

TEST(NAME, point_in_aabb)
{
    aabb_t bb = aabb(3, 1, 6, 4, 2, 7);
    WS_REAL p1[3] = {3, 1, 6};
    WS_REAL p2[3] = {4, 2, 7};
    WS_REAL p3[3] = {3.5, 1.5, 6.5};
    EXPECT_THAT(intersect_point_aabb_test(p1, bb.xyzxyz), Eq(true));
    EXPECT_THAT(intersect_point_aabb_test(p2, bb.xyzxyz), Eq(true));
    EXPECT_THAT(intersect_point_aabb_test(p3, bb.xyzxyz), Eq(true));
}

TEST(NAME, point_outside_aabb)
{
    aabb_t bb = aabb(3, 1, 6, 4, 2, 7);
    WS_REAL points[6][3] = {
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
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {-1, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 0, 1), 0, 0, 0),
        vertex(vec3(1, 0, 1), 0, 0, 0),
        vertex(vec3(0, 0, -1), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    ASSERT_THAT(result.count, Eq(1));
    EXPECT_THAT(result.shape[0].data.x, DoubleEq(0.0));
    EXPECT_THAT(result.shape[0].data.y, DoubleEq(0.0));
    EXPECT_THAT(result.shape[0].data.z, DoubleEq(0.0));
}

TEST(NAME, line_face_intersects_2)
{
    // This misses the triangle
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {-1, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 1, 0), 0, 0, 0),
        vertex(vec3(1, 1, 0), 0, 0, 0),
        vertex(vec3(0, -1, 0), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    ASSERT_THAT(result.count, Eq(1));
    EXPECT_THAT(result.shape[0].data.x, DoubleEq(0.0));
    EXPECT_THAT(result.shape[0].data.y, DoubleEq(0.0));
    EXPECT_THAT(result.shape[0].data.z, DoubleEq(0.0));
}

TEST(NAME, line_face_intersects_close)
{
    // This misses the triangle
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {-0.01, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 1, 0), 0, 0, 0),
        vertex(vec3(1, 1, 0), 0, 0, 0),
        vertex(vec3(0, -1, 0), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    ASSERT_THAT(result.count, Eq(1));
}

TEST(NAME, line_face_misses_1)
{
    // This misses the triangle
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {1, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 0, 1), 0, 0, 0),
        vertex(vec3(1, 0, 1), 0, 0, 0),
        vertex(vec3(0, 0, -1), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    EXPECT_THAT(result.count, Eq(0));
}

TEST(NAME, line_face_misses_2)
{
    // This misses the triangle
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {1, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 1, 0), 0, 0, 0),
        vertex(vec3(1, 1, 0), 0, 0, 0),
        vertex(vec3(0, -1, 0), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    EXPECT_THAT(result.count, Eq(0));
}

TEST(NAME, line_face_misses_close)
{
    // This misses the triangle
    WS_REAL p1[3] = {1, 1, 1};
    WS_REAL p2[3] = {0.01, -1, -1};
    face_t f = face(
        vertex(vec3(-1, 1, 0), 0, 0, 0),
        vertex(vec3(1, 1, 0), 0, 0, 0),
        vertex(vec3(0, -1, 0), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    EXPECT_THAT(result.count, Eq(0));
}

TEST(NAME, line_face_segment_too_short)
{
    // Line ends before hitting the face
    WS_REAL p1[3] = {3, 3, 3};
    WS_REAL p2[3] = {0.01, 0.01, 0.01};
    face_t f = face(
        vertex(vec3(-1, 0, 1), 0, 0, 0),
        vertex(vec3(1, 0, 1), 0, 0, 0),
        vertex(vec3(0, 0, -1), 0, 0, 0)
    );
    intersect_result_t result = intersect_line_face(p1, p2, &f);
    EXPECT_THAT(result.count, Eq(0));
}


TEST(NAME, line_face_parallel_to_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
}

TEST(NAME, line_face_lines_inside_triangle_plane)
{
    ASSERT_THAT(true, Eq(false));
}

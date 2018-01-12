#include "gmock/gmock.h"
#include "wavesim/octree.h"
#include "wavesim/mesh.h"
#include "utils.hpp"

#define NAME octree

using namespace testing;

class NAME : public Test
{
public:
    virtual void SetUp() OVERRIDE
    {
        o = octree_create();
        m = NULL;
    }

    virtual void TearDown() OVERRIDE
    {
        octree_destroy(o);
        if (m) mesh_destroy(m);
    }

protected:
    octree_t* o;
    mesh_t* m;
};

TEST_F(NAME, build_from_empty_mesh)
{
    m = mesh_create();
    EXPECT_THAT(octree_build_from_mesh(o, m, vec3(0, 0, 0)), Eq(0));
}

TEST_F(NAME, build_from_mesh_with_one_face)
{
    mesh_builder_t* mb = mesh_builder_create();
    mesh_builder_add_face(mb, face(
        vertex(vec3(-1, -1, -1), 0, 0, 0),
        vertex(vec3(1, 1, 1), 0, 0, 0),
        vertex(vec3(0.4, -0.2, 0.8), 0, 0, 0)
    ));
    m = mesh_builder_finalize(mb);
    mesh_builder_destroy(mb);

    EXPECT_THAT(octree_build_from_mesh(o, m, vec3(0, 0, 0)), Eq(1));
    // Check boundaries
    EXPECT_THAT(AABB_AX(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AY(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AZ(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_BX(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BY(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BZ(o->root.aabb), DoubleEq(1));
    // Octree should contain the single face
    ASSERT_THAT(octree_face_count(o), Eq(1));
}

TEST_F(NAME, build_from_cube_mesh)
{
    m = mesh_create();
    mesh_cube(m, aabb(-1, -1, -1, 1, 1, 1));
    EXPECT_THAT(octree_build_from_mesh(o, m, vec3(0, 0, 0)), Eq(12)); // Cube has 12 polygons
    // Check boundaries
    EXPECT_THAT(AABB_AX(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AY(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AZ(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_BX(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BY(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BZ(o->root.aabb), DoubleEq(1));
    // Octree should contain all 12 faces
    ASSERT_THAT(octree_face_count(o), Eq(12));
}

TEST_F(NAME, cube_mesh_with_small_triangles)
{
    mesh_builder_t* mb = mesh_builder_create();
    mesh_builder_cube(mb, aabb(-1, -1, -1, 1, 1, 1));
    mesh_builder_add_face(mb, face(
        vertex(vec3(0.01, 0.01, 0.01), 0, 0, 0),
        vertex(vec3(0.01, 0.01, 0.02), 0, 0, 0),
        vertex(vec3(0.01, 0.02, 0.02), 0, 0, 0)
    ));
    mesh_builder_add_face(mb, face(
        vertex(vec3(0.03, 0.03, 0.03), 0, 0, 0),
        vertex(vec3(0.03, 0.04, 0.04), 0, 0, 0),
        vertex(vec3(0.03, 0.04, 0.04), 0, 0, 0)
    ));
    m = mesh_builder_finalize(mb);
    mesh_builder_destroy(mb);

    EXPECT_THAT(octree_build_from_mesh(o, m, vec3(0, 0, 0)), Eq(25));
    // Check boundaries
    EXPECT_THAT(AABB_AX(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AY(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_AZ(o->root.aabb), DoubleEq(-1));
    EXPECT_THAT(AABB_BX(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BY(o->root.aabb), DoubleEq(1));
    EXPECT_THAT(AABB_BZ(o->root.aabb), DoubleEq(1));
    // Octree top node should contain all 14 faces
    ASSERT_THAT(octree_face_count(o), Eq(14));
}

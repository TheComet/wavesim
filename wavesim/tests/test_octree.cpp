#include "gmock/gmock.h"
#include "wavesim/obj.h"
#include "wavesim/octree.h"
#include "wavesim/mesh.h"
#include "utils.hpp"

#define NAME octree

using namespace testing;

class NAME : public Test
{
public:
    virtual void SetUp()
    {
        octree_create(&o);
        m = NULL;
    }

    virtual void TearDown()
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
    mesh_create(&m);
    EXPECT_THAT(octree_build_from_mesh(o, m, 6), Eq(0));
}

TEST_F(NAME, large_and_small_triangles)
{
    mesh_builder_t* mb; mesh_builder_create(&mb);
    mesh_builder_add_face(mb, face(
        vertex(vec3(-1, -1, -1), attribute_default()),
        vertex(vec3(1, 1, 1), attribute_default()),
        vertex(vec3(1, 1, -1), attribute_default())
    ));
    mesh_builder_add_face(mb, face(
        vertex(vec3(-1, -1, -1), attribute_default()),
        vertex(vec3(-0.9, -0.9, -0.9), attribute_default()),
        vertex(vec3(-1, -0.9, -0.9), attribute_default())
    ));
    EXPECT_THAT(mesh_builder_build(&m, mb), Eq(WS_OK));
    mesh_builder_destroy(mb);

    EXPECT_THAT(octree_build_from_mesh(o, m, 6), Eq(WS_OK));
    EXPECT_THAT(obj_export_octree("octree.large_and_small_triangles.obj", o), Eq(WS_OK));
}

TEST_F(NAME, build_from_cube_mesh)
{
    EXPECT_THAT(mesh_create(&m), Eq(WS_OK));
    mesh_cube(m, aabb(-1, -1, -1, 1, 1, 1));
    EXPECT_THAT(octree_build_from_mesh(o, m, 6), Eq(WS_OK));
    EXPECT_THAT(obj_export_octree("octree.build_from_cube_mesh.obj", o), Eq(WS_OK));
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
    mesh_builder_t* mb; mesh_builder_create(&mb);
    mesh_builder_cube(mb, aabb(-1, -1, -1, 1, 1, 1));
    mesh_builder_add_face(mb, face(
        vertex(vec3(0.01, 0.01, 0.01), attribute_default()),
        vertex(vec3(0.01, 0.01, 0.02), attribute_default()),
        vertex(vec3(0.01, 0.02, 0.02), attribute_default())
    ));
    mesh_builder_add_face(mb, face(
        vertex(vec3(0.03, 0.03, 0.03), attribute_default()),
        vertex(vec3(0.03, 0.04, 0.04), attribute_default()),
        vertex(vec3(0.03, 0.04, 0.04), attribute_default())
    ));
    EXPECT_THAT(mesh_builder_build(&m, mb), Eq(WS_OK));
    mesh_builder_destroy(mb);

    EXPECT_THAT(octree_build_from_mesh(o, m, 6), Eq(WS_OK));
    EXPECT_THAT(obj_export_octree("octree.cube_mesh_with_small_triangles.obj", o), Eq(WS_OK));
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

/*
TEST_F(NAME, from_cube_obj)
{
    mesh_create(&m);
    obj_import_mesh("../wavesim/models/cube.obj", m);
    octree_build_from_mesh(o, m, vec3(1, 1, 1));
    obj_export_octree("octree.from_cube_obj.obj", o);
}

TEST_F(NAME, from_high_ceiling_obj)
{
    mesh_create(&m);
    EXPECT_THAT(obj_import_mesh("../wavesim/models/high-ceiling.obj", m), Eq(WS_OK));
    octree_build_from_mesh(o, m, vec3(2, 2, 2));
    obj_export_octree("octree.from_high_ceiling_obj.obj", o);
}*/

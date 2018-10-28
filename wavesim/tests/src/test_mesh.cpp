#include "gmock/gmock.h"
#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/obj.h"
#include "utils.hpp"

#define NAME mesh

using namespace ::testing;

TEST(NAME, todo)
{
    EXPECT_THAT(true, Eq(false));
}

TEST(NAME, check_empty_mesh_is_non_manifold)
{
    mesh_t* m;
    ASSERT_THAT(mesh_create(&m, "test"), Eq(WS_OK));
    EXPECT_THAT(mesh_is_manifold(m), Eq(0));
    mesh_destroy(m);
}

TEST(NAME, check_cube_is_manifold)
{
    mesh_t* m;
    ASSERT_THAT(mesh_create(&m, "test"), Eq(WS_OK));
    mesh_cube(m, aabb(0, 0, 0, 1, 1, 1));
    EXPECT_THAT(mesh_is_manifold(m), Eq(1));
    mesh_destroy(m);
}

TEST(NAME, check_triangle_is_non_manifold)
{
    mesh_t* m;
    mesh_builder_t* mb;
    ASSERT_THAT(mesh_builder_create(&mb), Eq(WS_OK));
    mesh_builder_add_face(mb, face(
        vertex(vec3(0.01, 0.01, 0.01), attribute_default_air()),
        vertex(vec3(0.01, 0.01, 0.02), attribute_default_air()),
        vertex(vec3(0.01, 0.02, 0.02), attribute_default_air())
    ));
    ASSERT_THAT(mesh_builder_build(&m, mb), Eq(WS_OK));
    EXPECT_THAT(mesh_is_manifold(m), Eq(0));
    mesh_builder_destroy(mb);
    mesh_destroy(m);
}

TEST(NAME, check_cube_model_is_manifold)
{
    mesh_t* m;
    ASSERT_THAT(mesh_create(&m, "cube.obj"), Eq(WS_OK));
    EXPECT_THAT(obj_import_mesh("../wavesim/models/cube.obj", m), Eq(WS_OK));
    EXPECT_THAT(mesh_is_manifold(m), Eq(1));
    mesh_destroy(m);
}


TEST(NAME, check_high_ceiling_model_is_manifold)
{
    mesh_t* m;
    ASSERT_THAT(mesh_create(&m, "high-ceiling.obj"), Eq(WS_OK));
    EXPECT_THAT(obj_import_mesh("../wavesim/models/high-ceiling.obj", m), Eq(WS_OK));
    EXPECT_THAT(mesh_is_manifold(m), Eq(1));
    mesh_destroy(m);
}

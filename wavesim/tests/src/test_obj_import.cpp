#include "gmock/gmock.h"
#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/obj.h"

#define NAME obj_import

using namespace ::testing;

TEST(NAME, try_import_cube)
{
    mesh_t* m;
    ASSERT_THAT(mesh_create(&m, "test"), Eq(WS_OK));
    ASSERT_THAT(obj_import_mesh("../wavesim/models/cube.obj", m), Eq(WS_OK));

    /* This particular cube spans from [-5, 0, -5] to [5, 10, 5] */
    EXPECT_THAT(AABB_AX(m->aabb), DoubleEq(-5));
    EXPECT_THAT(AABB_AY(m->aabb), DoubleEq(0));
    EXPECT_THAT(AABB_AZ(m->aabb), DoubleEq(-5));
    EXPECT_THAT(AABB_BX(m->aabb), DoubleEq(5));
    EXPECT_THAT(AABB_BY(m->aabb), DoubleEq(10));
    EXPECT_THAT(AABB_BZ(m->aabb), DoubleEq(5));

    /* There should be 8 vertices, 12 faces, and 36 indices */
    EXPECT_THAT(mesh_vertex_count(m), Eq(8));
    EXPECT_THAT(mesh_face_count(m), Eq(12));
    EXPECT_THAT(mesh_index_count(m), Eq(36));

    mesh_destroy(m);
}

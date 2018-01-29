#include "gmock/gmock.h"
#include "wavesim/medium.h"
#include "wavesim/mesh.h"
#include "wavesim/obj.h"

#define NAME medium

using namespace ::testing;

TEST(NAME, high_ceiling)
{
    mesh_t* mesh;
    medium_t* medium;
    vec3_t grid_size = vec3(2, 2, 2);
    ASSERT_THAT(mesh_create(&mesh), Eq(WS_OK));
    ASSERT_THAT(obj_import_mesh("../wavesim/models/high-ceiling.obj", mesh), Eq(WS_OK));
    ASSERT_THAT(medium_create(&medium), Eq(WS_OK));
    ASSERT_THAT(medium_build_from_mesh(medium, NULL, mesh, grid_size.xyz), Eq(WS_OK));
    EXPECT_THAT(obj_export_medium("medium.high_ceiling.obj", medium), Eq(WS_OK));

    medium_destroy(medium);
    mesh_destroy(mesh);
}

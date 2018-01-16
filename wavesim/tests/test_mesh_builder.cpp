#include "gmock/gmock.h"
#include "wavesim/mesh_builder.h"
#include "wavesim/mesh.h"

#define NAME mesh_builder

using namespace ::testing;

TEST(NAME, build_empty_mesh_and_clear_doesnt_crash)
{
    mesh_builder_t* mb = mesh_builder_create();
    mesh_t* m = mesh_builder_build(mb);
    mesh_builder_destroy(mb);
    mesh_clear_buffers(m);
    mesh_destroy(m);
}

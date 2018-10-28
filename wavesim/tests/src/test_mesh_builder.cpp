#include "gmock/gmock.h"
#include "wavesim/mesh/mesh_builder.h"
#include "wavesim/mesh/mesh.h"

#define NAME mesh_builder

using namespace ::testing;

TEST(NAME, build_empty_mesh_and_clear_doesnt_crash)
{
    mesh_builder_t* mb; mesh_builder_create(&mb);
    mesh_t* m; mesh_builder_build(&m, mb);
    mesh_builder_destroy(mb);
    mesh_clear_buffers(m);
    mesh_destroy(m);
}

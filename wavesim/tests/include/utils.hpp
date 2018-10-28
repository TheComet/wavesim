#pragma once

#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/mesh_builder.h"

void mesh_builder_cube(mesh_builder_t* mb, aabb_t bb);
void mesh_cube(mesh_t* mesh, aabb_t bb);

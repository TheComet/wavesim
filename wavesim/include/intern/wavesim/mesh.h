#ifndef MESH_H
#define MESH_H

#include "wavesim/config.h"
#include "wavesim/vector.h"

typedef struct mesh_t
{
    vector_t faces;  /* holds face_t instances */
} mesh_t;

WAVESIM_PRIVATE_API mesh_t*
mesh_create();

WAVESIM_PRIVATE_API void
mesh_construct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destruct(mesh_t* mesh);

WAVESIM_PRIVATE_API void
mesh_destroy(mesh_t* mesh);

#endif /* MESH_H */

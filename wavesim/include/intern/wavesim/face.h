#ifndef FACE_H
#define FACE_H

#include "wavesim/config.h"
#include "wavesim/vertex.h"

C_BEGIN

typedef struct face_t
{
    vertex_t vertices[3];
} face_t;

WAVESIM_PRIVATE_API face_t
face(vertex_t v1, vertex_t v2, vertex_t v3);

C_END

#endif /* FACE_H */

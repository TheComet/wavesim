#ifndef FACE_H
#define FACE_H

#include "wavesim/config.h"
#include "wavesim/vertex.h"
#include "wavesim/aabb.h"

C_BEGIN

typedef struct face_t
{
    vertex_t vertices[3];
} face_t;

WAVESIM_PRIVATE_API face_t
face(vertex_t v1, vertex_t v2, vertex_t v3);

WAVESIM_PRIVATE_API int
face_is_same(const face_t* f1, const face_t* f2);

WAVESIM_PRIVATE_API aabb_t
face_calculate_aabb(const face_t* face);

WAVESIM_PRIVATE_API void
face_interpolate_attributes_barycentric(const face_t* face, attribute_t* attr, const WS_REAL bary[3]);

C_END

#endif /* FACE_H */

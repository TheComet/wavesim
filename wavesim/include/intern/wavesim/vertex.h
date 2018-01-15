#ifndef VERTEX_H
#define VERTEX_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"
#include "wavesim/attribute.h"

C_BEGIN

typedef struct vertex_t
{
    vec3_t position;
    attribute_t attr;

} vertex_t;

WAVESIM_PRIVATE_API vertex_t
vertex(vec3_t position, attribute_t attribute);

WAVESIM_PRIVATE_API int
vertex_is_same(const vertex_t* v1, const vertex_t* v2);

C_END

#endif /* VERTEX_H */

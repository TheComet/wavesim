#ifndef VERTEX_H
#define VERTEX_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

typedef struct vertex_attrs_t
{
    real reflection;
    real transmission;
    real absorbtion;
} vertex_attrs_t;

typedef struct vertex_t
{
    vec3_t position;
    vertex_attrs_t attrs;

} vertex_t;

WAVESIM_PRIVATE_API vertex_t
vertex(vec3_t position, real reflection, real transmission, real absorbtion);

C_END

#endif /* VERTEX_H */

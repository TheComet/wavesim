#ifndef FACE_H
#define FACE_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

typedef struct surface_properties_t
{
    real reflection;
    real absorbtion;
    real transmission;
} surface_properties_t;

typedef struct face_t
{
    surface_properties_t surface_properties;
    vec3_t vertices[3];
} face_t;

#endif /* FACE_H */

#include "wavesim/vertex.h"

/* ------------------------------------------------------------------------- */
vertex_t
vertex(vec3_t position, WS_REAL reflection, WS_REAL transmission, WS_REAL absorbtion)
{
    return (vertex_t){position, {reflection, transmission, absorbtion}};
}

/* ------------------------------------------------------------------------- */
int
vertex_is_same(const vertex_t* v1, const vertex_t* v2)
{
    int i;
    if (v1->attr.absorbtion != v2->attr.absorbtion)
        return 0;
    if (v1->attr.reflection != v2->attr.reflection)
        return 0;
    if (v1->attr.transmission != v2->attr.transmission)
        return 0;
    for (i = 0; i != 3; ++i)
        if (v1->position.xyz[i] != v2->position.xyz[i])
        return 0;

    return 1;
}

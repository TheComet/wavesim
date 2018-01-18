#include "wavesim/vertex.h"

/* ------------------------------------------------------------------------- */
vertex_t
vertex(vec3_t position, attribute_t attribute)
{
    vertex_t vert;
    vert.position = position;
    vert.attr = attribute;
    return vert;
}

/* ------------------------------------------------------------------------- */
int
vertex_is_same(const vertex_t* v1, const vertex_t* v2)
{
    int i;
    if (attribute_is_same(&v1->attr, &v2->attr) == 0)
        return 0;
    for (i = 0; i != 3; ++i)
        if (v1->position.xyz[i] != v2->position.xyz[i])
        return 0;

    return 1;
}

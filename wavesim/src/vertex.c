#include "wavesim/vertex.h"

/* ------------------------------------------------------------------------- */
vertex_t
vertex(vec3_t position, real reflection, real transmission, real absorbtion)
{
    return (vertex_t){position, {reflection, transmission, absorbtion}};
}

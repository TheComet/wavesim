#include "wavesim/face.h"

/* ------------------------------------------------------------------------- */
face_t
face(vertex_t v1, vertex_t v2, vertex_t v3)
{
    return (face_t){{v1, v2, v3}};
}

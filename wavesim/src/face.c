#include "wavesim/face.h"

/* ------------------------------------------------------------------------- */
face_t
face(vertex_t v1, vertex_t v2, vertex_t v3)
{
    face_t f;
    f.vertices[0] = v1;
    f.vertices[1] = v2;
    f.vertices[2] = v3;
    return f;
}

/* ------------------------------------------------------------------------- */
int
face_is_same(const face_t* f1, const face_t* f2)
{
    int i;
    for (i = 0; i != 3; ++i)
    {
        if (vertex_is_same(&f1->vertices[i], &f2->vertices[i]) == 0)
            return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
aabb_t
face_calculate_aabb(const face_t* face)
{
    int v, a; /* vertex, axis */
    aabb_t ret = aabb_reset();
    for (v = 0; v != 3; ++v)
        for (a = 0; a != 3; ++a)
        {
            if (face->vertices[v].position.xyz[a] < ret.b.min.xyz[a])
                ret.b.min.xyz[a] = face->vertices[v].position.xyz[a];
            if (face->vertices[v].position.xyz[a] > ret.b.max.xyz[a])
                ret.b.max.xyz[a] = face->vertices[v].position.xyz[a];
        }

    return ret;
}

/* ------------------------------------------------------------------------- */
void
face_interpolate_attributes_barycentric(const face_t* face, attribute_t* attr, const wsreal_t bary[3])
{
    int i;

    attribute_set_zero(attr);
    for (i = 0; i != 3; ++i)
    {
        attr->absorption   += face->vertices[i].attr.absorption   * bary[i];
        attr->reflection   += face->vertices[i].attr.reflection   * bary[i];
        attr->transmission += face->vertices[i].attr.transmission * bary[i];
    }
}

#include "wavesim/hash.h"

/* ------------------------------------------------------------------------- */
hash32_t
hash32_jenkins_oaat(const void* key, uintptr_t len)
{
    hash32_t hash, i;
    for(hash = i = 0; i != len; ++i)
    {
        hash += *((uint8_t*)key + i);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 1);
    hash += (hash << 15);
    return hash;
}

/* ------------------------------------------------------------------------- */
hash32_t
hash32_combine(hash32_t lhs, hash32_t rhs)
{
    lhs^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

/* ------------------------------------------------------------------------- */
hash32_t
hash32_vec3(const wsreal_t v[3])
{
    int i, j;
    hash32_t hash = 0;
    for (i = 0; i != 3; ++i) /* for every component (xyz) in vector */
        for (j = 0; j != sizeof(wsreal_t)/4; ++j) /* for every 32-bit chunk */
            hash = hash32_combine(hash, ((hash32_t*)&v[i])[j]);
    return hash;
}

/* ------------------------------------------------------------------------- */
hash32_t
hash32_index(const wsib_t index)
{
    int i;
    hash32_t hash = 0;
    for (i = 0; i != sizeof(wsib_t)/4; ++i) /* for every 32-bit chunk */
        hash = hash32_combine(hash, ((hash32_t*)&index)[i]);
    return hash;
}

/* ------------------------------------------------------------------------- */
hash32_t
hash32_edge_indices(const wsib_t indices[2])
{
    return hash32_combine(
        hash32_index(indices[0]),
        hash32_index(indices[1]));
}

/* ------------------------------------------------------------------------- */
hash32_t
hash32_face_indices(const wsib_t indices[3])
{
    return hash32_combine(
        hash32_combine(
            hash32_index(indices[0]),
            hash32_index(indices[1])),
            hash32_index(indices[2]));
}

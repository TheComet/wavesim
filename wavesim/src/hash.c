#include "wavesim/hash.h"

/* ------------------------------------------------------------------------- */
uint32_t
hash_combine(uint32_t lhs, uint32_t rhs)
{
    lhs^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

/* ------------------------------------------------------------------------- */
uint32_t
hash_vec3(const WS_REAL v[3])
{
    int i, j;
    uint32_t hash = 0;
    for (i = 0; i != 3; ++i) /* for every component (xyz) in vector */
        for (j = 0; j != sizeof(WS_REAL)/4; ++j) /* for every 32-bit chunk */
            hash = hash_combine(hash, ((uint32_t*)&v[i])[j]);
    return hash;
}

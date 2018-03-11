#ifndef WAVESIM_HASH_H
#define WAVESIM_HASH_H

#include "wavesim/config.h"

C_BEGIN

typedef uint32_t hash_t;
typedef hash_t (*hash_func)(const void*, size_t);

WAVESIM_PRIVATE_API hash_t
hash_jenkins_oaat(const void* key, size_t len);

/*!
 * @brief Taken from boost::hash_combine. Combines two hash values into a
 * new hash value.
 */
WAVESIM_PRIVATE_API hash_t
hash_combine(hash_t lhs, hash_t rhs);

WAVESIM_PRIVATE_API hash_t
hash_vec3(const wsreal_t v[3]);

WAVESIM_PRIVATE_API hash_t
hash_index(const wsib_t index);

WAVESIM_PRIVATE_API hash_t
hash_edge_indices(const wsib_t indices[2]);

WAVESIM_PRIVATE_API hash_t
hash_face_indices(const wsib_t indices[3]);

C_END

#endif /* WAVESIM_HASH_H */

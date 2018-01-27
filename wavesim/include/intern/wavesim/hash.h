#ifndef WAVESIM_HASH_H
#define WAVESIM_HASH_H

#include "wavesim/config.h"

C_BEGIN

/*!
 * @brief Taken from boost::hash_combine. Combines two hash values into a
 * new hash value.
 */
WAVESIM_PRIVATE_API uint32_t
hash_combine(uint32_t lhs, uint32_t rhs);

WAVESIM_PRIVATE_API uint32_t
hash_vec3(const wsreal_t v[3]);

WAVESIM_PRIVATE_API uint32_t
hash_index(const wsib_t index);

WAVESIM_PRIVATE_API uint32_t
hash_face_indices(const wsib_t index[3]);

C_END

#endif /* WAVESIM_HASH_H */

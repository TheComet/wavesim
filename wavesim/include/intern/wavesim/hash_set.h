#ifndef WAVESIM_SET_H
#define WAVESIM_SET_H

#include "wavesim/config.h"
#include "wavesim/hash.h"

C_BEGIN

#define HASH_SET_OUT_OF_MEMORY (hash_t)-1
#define HASH_SET_ERROR (hash_t)-2

#define SLOT_UNUSED (hash_t)0
#define SLOT_TOMBSTONE (hash_t)1

typedef enum hash_set_flags_e
{
    HM_INSERTION_HEAVY = 0x01,
    HM_REFERENCE_KEYS  = 0x02,
    HM_STRINGS         = 0x04,
    HM_BINARY          = 0x06,
    HM_INTEGERS        = 0x08
} hash_set_flags_e;

typedef struct key_store_t
{
    /*! Allocates the array in which unhashed keys can be stored
     * @param[in] table_size The table size of the set. Implementations should
     * size the key store to match this size.
     * @return Returns a pointer to the allocated array. Depending on how this
     * gets implemented, it may be an array of arrays, or it may just be a flat
     * contiguous chunk of memory. */
    void** (*alloc)(hash_t table_size);

    /*! Frees the array in which unhashed keys are stored.
     * @param[in] keys The pointer to the array that was returned by alloc()
     * @param[in] table_size The table size of the set. */
    void (*free)(void** keys, hash_t table_size);

    void (*load)(hash_t home, void** keys, void** data, size_t* len);
    /*! Stores some data into the key store. */
    int (*store)(hash_t home, void** keys, const void* data, size_t len);
    void (*erase)(hash_t home, void** keys);

    hash_t (*find_existing)(hash_t key,
                       const hash_t* table, void** keys, hash_t table_size,
                       const void* data, size_t len);
    hash_t (*find_new)(hash_t key,
                       const hash_t* table, void** keys, hash_t table_size,
                       const void* data, size_t len);
} key_store_t;

extern key_store_t generic_key_store;
extern key_store_t ref_key_store;
extern key_store_t contiguous_key_store;

typedef struct hash_set_t
{
    hash_t* table;
    void** keys;
    hash_t table_size;
    hash_t slots_used;
    hash_func hash;
    key_store_t key_store;
} hash_set_t;

WAVESIM_PRIVATE_API wsret
hash_set_create(hash_set_t** hs, uint8_t flags);

WAVESIM_PRIVATE_API void
hash_set_destroy(hash_set_t* hs);

WAVESIM_PRIVATE_API wsret
hash_set_construct(hash_set_t* hs, uint8_t flags);

WAVESIM_PRIVATE_API void
hash_set_destruct(hash_set_t* hs);

/*!
 * @brief Adds data to the set. The data is hashed using an appropriate
 * algorithm depending on whether HM_STRINGS, HM_BINARY or HM_INTEGERS was
 * specified during construction.
 * @param[in] hs the set to add data to.
 * @param[in] data The data to hash and insert. If HM_REFERENCE_KEYS was
 * specified during creation, only pointers to the data are stored in the set.
 * Otherwise, the data is copied into the set.
 * @param[in] len the length in bytes of the data to insert.
 * @return Upon successful insertion, the insertion index into the bucket is
 * returned (a value between 0 and bucket_size-1). If the data already exists
 * in the set, nothing is changed and HASH_SET_ERROR is returned.  If memory
 * fails to be allocated, HASH_SET_OUT_OF_MEMORY is returned.
 */
WAVESIM_PRIVATE_API hash_t
hash_set_add(hash_set_t* hs, const void* data, size_t len);

/*!
 * @brief Searches the hash set for the specified data. The data is hash
 * using an appropriate algorithm depending on whether HM_STRINGS, HM_BINARY or
 * HM_INTEGERS was specified during construction.
 * @param[in] hs The set to search.
 * @param[in] data The data to search for.
 * @param[in] len The length in bytes of the data.
 * @return If the data is found, the insertion index into the bucket is
 * returned (a value between 0 and bucket_size-1). If the data already exists
 * in the set, nothing is changed and HASH_SET_ERROR is returned
 */
WAVESIM_PRIVATE_API hash_t
hash_set_find(const hash_set_t* hs, const void* data, size_t len);

/*!
 * @brief Searches the hash set for the specified data and removes it. The data
 * is hash using an appropriate algorithm depending on whether HM_STRINGS,
 * HM_BINARY or HM_INTEGERS was specified during construction.
 * @param[in] hs The set to remove data from.
 * @param[in] data The data to search for.
 * @param[in] len The length in bytes of the data.
 * @return If the data is found, the insertion index into the bucket is
 * returned (a value between 0 and bucket_size-1) and the data is removed from
 * the set. If the data was not found, then HASH_SET_ERROR is returned and the
 * set is not modified.
 */
WAVESIM_PRIVATE_API hash_t
hash_set_remove(hash_set_t* hs, const void* data, size_t len);

/*!
 * @brief String version of hash_set_add(). Behaves exactly the same.
 */
WAVESIM_PRIVATE_API hash_t
hash_set_add_str(hash_set_t* hs, const char* str);

/*!
 * @brief String version of hash_set_find(). Behaves exactly the same.
 */
WAVESIM_PRIVATE_API hash_t
hash_set_find_str(const hash_set_t* hs, const char* str);

/*!
 * @brief String version of hash_set_remove(). Behaves exactly the same.
 */
WAVESIM_PRIVATE_API hash_t
hash_set_remove_str(hash_set_t* hs, const char* str);

/*!
 * @brief Clears the set of all data.
 */
WAVESIM_PRIVATE_API void
hash_set_clear(hash_set_t* hs);

#define hash_set_count(hs) (hs->slots_used)

C_END

#endif /* WAVESIM_SET_H */

#ifndef WAVESIM_MAP_H
#define WAVESIM_MAP_H

#include "wavesim/config.h"
#include "wavesim/hash.h"

#define HM_SLOT_UNUSED    ((hash32_t)0)
#define HM_SLOT_TOMBSTONE ((hash32_t)1)
#define HM_DEFAULT_TABLE_COUNT 128

C_BEGIN

typedef struct hashmap_t
{
    hash32_t     table_count;
    hash32_t     key_size;
    hash32_t     value_size;
    hash32_t     slots_used;
    hash32_func  hash;
    void*        storage;
} hashmap_t;

WAVESIM_PRIVATE_API wsret
hashmap_create(hashmap_t** hm, hash32_t key_size, hash32_t value_size);

WAVESIM_PRIVATE_API wsret
hashmap_construct(hashmap_t* hm, hash32_t key_size, hash32_t value_size);

WAVESIM_PRIVATE_API void
hashmap_destruct(hashmap_t* hm);

WAVESIM_PRIVATE_API void
hashmap_destroy(hashmap_t* hm);

WAVESIM_PRIVATE_API wsret
hashmap_insert(hashmap_t* hm, const void* key, const void* value);

WAVESIM_PRIVATE_API wsret
hashmap_insert_str(hashmap_t* hm, const char* key, const void* value);

WAVESIM_PRIVATE_API void*
hashmap_erase(hashmap_t* hm, const void* key);

WAVESIM_PRIVATE_API void*
hashmap_erase_str(hashmap_t* hm, const char* key);

WAVESIM_PRIVATE_API void*
hashmap_find(const hashmap_t* hm, const void* key);

WAVESIM_PRIVATE_API void*
hashmap_find_str(hashmap_t* hm, const char* key);

#define hashmap_count(hm) ((hm)->slots_used)

C_END

#endif /* WAVESIM_MAP_H */

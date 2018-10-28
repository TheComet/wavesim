#include "wavesim/hashmap.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

#define SLOT(hm, pos)  (*(hash32_t*)((uint8_t*)hm->storage + (sizeof(hash32_t) + hm->key_size) * pos))
#define KEY(hm, pos)   ((void*)((uint8_t*)hm->storage + (sizeof(hash32_t) + hm->key_size) * pos + sizeof(hash32_t)))
#define VALUE(hm, pos) ((void*)((uint8_t*)hm->storage + (sizeof(hash32_t) + hm->key_size) * hm->table_count + hm->value_size * pos))

/* ------------------------------------------------------------------------- */
/*
 * Need to account for the possibility that our hash function will produce
 * reserved values. In this case, return a value that is not reserved in a
 * predictable way.
 */
static hash32_t
hash_wrapper(const hashmap_t* hm, const void* data, hash32_t len)
{
    hash32_t hash = hm->hash(data, len);
    if (hash == HM_SLOT_UNUSED || hash == HM_SLOT_TOMBSTONE)
        return 2;
    return hash;
}

/* ------------------------------------------------------------------------- */
static void*
malloc_and_init_storage(hash32_t key_size, hash32_t value_size, hash32_t table_count)
{
    /* Store the hashes, keys and values in one contiguous chunk of memory */
    void* storage = MALLOC((sizeof(hash32_t) + key_size + value_size) * table_count);
    if (storage == NULL)
        return NULL;

    /* Initialize hash table -- NOTE: Only works if HM_SLOT_UNUSED is 0 */
    memset(storage, 0, (sizeof(hash32_t) + key_size) * table_count);
    return storage;
}

/* ------------------------------------------------------------------------- */
static int
resize_rehash(hashmap_t* hm, hash32_t new_table_count)
{
    hashmap_t new_hm;
    hash32_t i;

    memcpy(&new_hm, hm, sizeof(hashmap_t));
    new_hm.table_count = new_table_count;
    new_hm.slots_used = 0;
    new_hm.storage = malloc_and_init_storage(hm->key_size, hm->value_size, new_table_count);
    if (new_hm.storage == NULL)
        return -1;

    for (i = 0; i != hm->table_count; ++i)
    {
        if (SLOT(hm, i) == HM_SLOT_UNUSED || SLOT(hm, i) == HM_SLOT_TOMBSTONE)
            continue;
        if (hashmap_insert(&new_hm, KEY(hm, i), VALUE(hm, i)) != HM_OK)
        {
            FREE(new_hm.storage);
            return -1;
        }
    }

    /* Swap storage and free old */
    FREE(hm->storage);
    hm->storage = new_hm.storage;
    hm->table_count = new_table_count;

    return 0;
}

/* ------------------------------------------------------------------------- */
hashmapret
hashmap_create(hashmap_t** hm, hash32_t key_size, hash32_t value_size)
{
    *hm = MALLOC(sizeof(**hm));
    if (*hm == NULL)
        return HM_OUT_OF_MEMORY;

    return hashmap_construct(*hm, key_size, value_size);
}

/* ------------------------------------------------------------------------- */
hashmapret
hashmap_construct(hashmap_t* hm, hash32_t key_size, hash32_t value_size)
{
    hm->key_size = key_size;
    hm->value_size = value_size;
    hm->hash = hash32_jenkins_oaat;
    hm->slots_used = 0;
    hm->table_count = HM_DEFAULT_TABLE_COUNT;
    hm->storage = malloc_and_init_storage(hm->key_size, hm->value_size, hm->table_count);
    if (hm->storage == NULL)
        return HM_OUT_OF_MEMORY;

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void
hashmap_destruct(hashmap_t* hm)
{
    FREE(hm->storage);
}

/* ------------------------------------------------------------------------- */
void
hashmap_destroy(hashmap_t* hm)
{
    hashmap_destruct(hm);
    FREE(hm);
}

/* ------------------------------------------------------------------------- */
hashmapret
hashmap_insert(hashmap_t* hm, const void* key, const void* value)
{
    hash32_t hash, pos, i, last_tombstone;

    /* NOTE: Rehashing may change table count, make sure to compute hash after this */
    if (hm->slots_used * 10 / hm->table_count >= 7)
        if (resize_rehash(hm, hm->table_count*2) != 0)
            return HM_OUT_OF_MEMORY;

    /* Init values */
    hash = hash_wrapper(hm, key, hm->key_size);
    pos = hash % hm->table_count;
    i = 0;
    last_tombstone = (hash32_t)-1;

    while (SLOT(hm, pos) != HM_SLOT_UNUSED)
    {
        /* If the same hash already exists in this slot, and this isn't the
         * result of a hash collision (which we can verify by comparing the
         * original keys), then we can conclude this key was already inserted */
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                return HM_KEY_EXISTS;
        }
        else
            if (SLOT(hm, pos) == HM_SLOT_TOMBSTONE)
                last_tombstone = pos;

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
    }

    if (last_tombstone != (hash32_t)-1)
        pos = last_tombstone;

    /* Store hash, key and value */
    SLOT(hm, pos) = hash;
    memcpy(KEY(hm, pos), key, hm->key_size);
    memcpy(VALUE(hm, pos), value, hm->value_size);

    hm->slots_used++;

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void*
hashmap_erase(hashmap_t* hm, const void* key)
{
    hash32_t hash = hash_wrapper(hm, key, hm->key_size);
    hash32_t pos = hash % hm->table_count;
    hash32_t i = 0;

    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                break;
        }
        else
        {
            if (SLOT(hm, pos) == HM_SLOT_UNUSED)
                return NULL;
        }

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
    }

    hm->slots_used--;

    SLOT(hm, pos) = HM_SLOT_TOMBSTONE;
    return VALUE(hm, pos);
}

/* ------------------------------------------------------------------------- */
void*
hashmap_find(const hashmap_t* hm, const void* key)
{
    hash32_t hash = hash_wrapper(hm, key, hm->key_size);
    hash32_t pos = hash % hm->table_count;
    hash32_t i = 0;
    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                break;
        }
        else
        {
            if (SLOT(hm, pos) == HM_SLOT_UNUSED)
                return NULL;
        }

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
    }

    return VALUE(hm, pos);
}

#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

/*
 * The generic key store implementation is an array of arrays. The outer array
 * has a fixed size (namely the bucket size) so it's easy to look up the
 * original keys. If an index in this array is unused, then it is NULL. For
 * each key that is stored, memory is allocated and a pointer to this memory is
 * assigned to a slot in the outer array.
 */

/* ------------------------------------------------------------------------- */
static void**
generic_ks_alloc(hash_t table_size)
{
    void** key_store = MALLOC(sizeof(void*) * table_size);
    if (key_store == NULL)
        return NULL;
    memset(key_store, 0, sizeof(void*) * table_size);
    return key_store;
}

/* ------------------------------------------------------------------------- */
static void
generic_ks_free(void** key_store, hash_t table_size)
{
    hash_t i;
    for (i = 0; i != table_size; ++i)
        if (key_store[i] != NULL)
            FREE(key_store[i]);
    FREE(key_store);
}

/* ------------------------------------------------------------------------- */
static hash_t
generic_ks_add(hash_t key,
               hash_t* table, void** key_store, hash_t table_size,
               const void* data, size_t len)
{
    hash_t i;
    hash_t home = key % table_size;
    hash_t tombstone = (hash_t)-1;
    for (i = 1; i != table_size + 1; ++i)
    {
        if (table[home] == SLOT_UNUSED)
            break;
        if (table[home] == SLOT_TOMBSTONE)
            tombstone = home;
        if (table[home] == key)
        {
            size_t cmp_len;
            memcpy(&cmp_len, key_store[home], sizeof(size_t));
            if (len == cmp_len &&
                    memcmp((uint8_t*)key_store[home] + sizeof(size_t), data, len) == 0)
                return HASH_SET_ERROR;
        }
        home += i;
        home = home % table_size;
    }
    assert(i != table_size+1);

    if (tombstone != (hash_t)-1)
        home = tombstone;

    /* Copy data into key storage. Make sure to save the data length as well */
    key_store[home] = MALLOC(sizeof(size_t) + len);
    if (key_store[home] == NULL)
        return HASH_SET_OUT_OF_MEMORY;
    memcpy(key_store[home], &len, sizeof(size_t));
    memcpy((uint8_t*)key_store[home] + sizeof(size_t), data, len);

    /* Store key in table */
    table[home] = key;

    return home;
}

/* ------------------------------------------------------------------------- */
static void
generic_ks_remove(hash_t key, hash_t* table, void** key_store)
{
    table[key] = SLOT_TOMBSTONE;
    FREE(key_store[key]);
    key_store[key] = NULL;
}

/* ------------------------------------------------------------------------- */
static hash_t
generic_ks_find(hash_t key,
                hash_t* table, void** key_store, hash_t table_size,
                const void* data, size_t len)
{
    hash_t i;
    hash_t home = key % table_size;
    for (i = 1; i != table_size+1; ++i)
    {
        if (table[home] == SLOT_UNUSED)
            break;
        if (table[home] == key)
        {
            size_t cmp_len;
            memcpy(&cmp_len, key_store[home], sizeof(size_t));
            if (cmp_len == len &&
                    memcmp((uint8_t*)key_store[home] + sizeof(size_t), data, len) == 0)
                return home;
        }
        home += i;
        home = home % table_size;
    }

    return HASH_SET_ERROR;
}

/* ------------------------------------------------------------------------- */
key_store_t generic_key_store = {
    generic_ks_alloc,
    generic_ks_free,
    generic_ks_add,
    generic_ks_remove,
    generic_ks_find
};

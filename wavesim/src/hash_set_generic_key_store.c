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
ksalloc(hash32_t table_size)
{
    void** key_store = MALLOC(sizeof(void*) * table_size);
    if (key_store == NULL)
        return NULL;
    memset(key_store, 0, sizeof(void*) * table_size);
    return key_store;
}

/* ------------------------------------------------------------------------- */
static void
ksfree(void** key_store, hash32_t table_size)
{
    hash32_t i;
    for (i = 0; i != table_size; ++i)
        if (key_store[i] != NULL)
            FREE(key_store[i]);
    FREE(key_store);
}

/* ------------------------------------------------------------------------- */
static void
load(hash32_t home, void** key_store, void** data, size_t* len)
{
    memcpy(len, key_store[home], sizeof(size_t));
    *data = (void*)((uint8_t*)key_store[home] + sizeof(size_t));

    assert(*data != NULL); /* Make sure set isn't accidentally reading an invalid slot */
}

/* ------------------------------------------------------------------------- */
static int
store(hash32_t home, void** key_store, const void* data, size_t len)
{
    assert(data != NULL);

    /* Copy data into key storage. Make sure to save the data length as well */
    key_store[home] = MALLOC(sizeof(size_t) + len);
    if (key_store[home] == NULL)
        return -1;

    memcpy(key_store[home], &len, sizeof(size_t));
    memcpy((uint8_t*)key_store[home] + sizeof(size_t), data, len);

    return 0;
}

/* ------------------------------------------------------------------------- */
static void
erase(hash32_t home, void** key_store)
{
    FREE(key_store[home]);
    key_store[home] = NULL;
}

/* ------------------------------------------------------------------------- */
static hash32_t
find_existing(hash32_t key,
              const hash32_t* table, void** key_store, hash32_t table_size,
              const void* data, size_t len)
{
    hash32_t i;
    hash32_t home = key % table_size;
    for (i = 1; i != table_size+1; ++i)
    {
        if (table[home] == HS_SLOT_UNUSED)
            break;
        if (table[home] == key)
        {
            size_t cmp_len;
            memcpy(&cmp_len, key_store[home], sizeof(size_t));
            if (cmp_len == len &&
                    memcmp((uint8_t*)key_store[home] + sizeof(size_t), data, len) == 0)
            {
                return home;
            }
        }
        home += i;
        home = home % table_size;
    }

    return HASH_SET_ERROR;
}

/* ------------------------------------------------------------------------- */
static hash32_t
find_new(hash32_t key,
         const hash32_t* table, void** key_store, hash32_t table_size,
         const void* data, size_t len)
{
    hash32_t i;
    hash32_t home = key % table_size;
    hash32_t tombstone = (hash32_t)-1;
    for (i = 1; i != table_size + 1; ++i)
    {
        if (table[home] == HS_SLOT_UNUSED)
            break;
        if (table[home] == HS_SLOT_TOMBSTONE)
            tombstone = home;
        if (table[home] == key)
        {
            size_t cmp_len;
            memcpy(&cmp_len, key_store[home], sizeof(size_t));
            if (len == cmp_len &&
                    memcmp((uint8_t*)key_store[home] + sizeof(size_t), data, len) == 0)
            {
                return HASH_SET_ERROR;
            }
        }
        home += i;
        home = home % table_size;
    }
    assert(i != table_size+1);

    if (tombstone != (hash32_t)-1)
        home = tombstone;

    return home;
}

/* ------------------------------------------------------------------------- */
key_store_t generic_key_store = {
    ksalloc,
    ksfree,
    load,
    store,
    erase,
    find_existing,
    find_new
};

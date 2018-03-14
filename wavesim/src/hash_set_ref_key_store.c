#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

/*
 * The "ref" key store implementation is an array of pointers that point to
 * the original, unhashed keys (without allocating and copying the data). The
 * length is stored along with it.
 */

typedef struct entry_t
{
    size_t len;
    void* data;
} entry_t;

/* ------------------------------------------------------------------------- */
static void**
ksalloc(hash_t table_size)
{
    void** key_store = MALLOC(sizeof(entry_t) * table_size);
    if (key_store == NULL)
        return NULL;
    memset(key_store, 0, sizeof(entry_t) * table_size);
    return key_store;
}

/* ------------------------------------------------------------------------- */
static void
ksfree(void** key_store, hash_t table_size)
{
    (void)table_size;
    FREE(key_store);
}

/* ------------------------------------------------------------------------- */
static void
load(hash_t home, void** key_store, void** data, size_t* len)
{
    entry_t* entry = (entry_t*)key_store + home;
    *data = entry->data;
    *len = entry->len;

    assert(*data != NULL);
}

/* ------------------------------------------------------------------------- */
static int
store(hash_t home, void** key_store, const void* data, size_t len)
{
    entry_t* entry = (entry_t*)key_store + home;
    entry->len = len;
    entry->data = (void*)data;

    assert(data != NULL);

    return 0;
}

/* ------------------------------------------------------------------------- */
static void
erase(hash_t home, void** key_store)
{
    memset((entry_t*)key_store + home, 0, sizeof(entry_t));
}

/* ------------------------------------------------------------------------- */
static hash_t
find_existing(hash_t key,
        const hash_t* table, void** key_store, hash_t table_size,
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
            entry_t* entry = (entry_t*)key_store + home;
            if (len == entry->len && memcmp(entry->data, data, len) == 0)
                return home;
        }
        home += i;
        home = home % table_size;
    }

    return HASH_SET_ERROR;
}

/* ------------------------------------------------------------------------- */
static hash_t
find_new(hash_t key,
        const hash_t* table, void** key_store, hash_t table_size,
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
            entry_t* entry = (entry_t*)key_store + home;
            if (len == entry->len && memcmp(entry->data, data, len) == 0)
                return HASH_SET_ERROR;
        }
        home += i;
        home = home % table_size;
    }
    assert(i != table_size+1);

    if (tombstone != (hash_t)-1)
        home = tombstone;

    return home;
}

/* ------------------------------------------------------------------------- */
key_store_t ref_key_store = {
    ksalloc,
    ksfree,
    load,
    store,
    erase,
    find_existing,
    find_new
};

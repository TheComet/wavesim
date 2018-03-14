#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

/*
 * The contiguous key store implementation is a single contiguous array capable
 * of storing variable length data. It expands by a factor of 2 every time it
 * needs to grow. There is a jump table located at the beginning of the key
 * store, which stores pointers to the actual data located further on in the
 * buffer.
 */

typedef struct header_t
{
    size_t capacity;
    size_t used;
    void* free;
} header_t;

typedef struct meta_t
{
    size_t len;
} meta_t;

#define HEADER(keys)  \
    ((header_t*)keys)

#define JUMP_TABLE(keys) \
    ((void**)((uint8_t*)keys + sizeof(header_t)))

/* ------------------------------------------------------------------------- */
static void**
ksalloc(hash_t table_size)
{
    void** pkeys;
    size_t size =
        sizeof(header_t) +           /* header, stores total length of allocation */
        sizeof(void*) * table_size + /* jump table */
        table_size;                  /* initial size for data */
    pkeys = MALLOC(sizeof(void*));
    if (pkeys == NULL)
        return NULL;
    *pkeys = MALLOC(size);
    if (*pkeys == NULL)
    {
        FREE(pkeys);
        return NULL;
    }
    memset(*pkeys, 0, sizeof(void*) * table_size);

    HEADER(*pkeys)->capacity = size;
    HEADER(*pkeys)->used = sizeof(header_t) +
                          sizeof(void*) * table_size;
    HEADER(*pkeys)->free = (void*)((uint8_t*)(*pkeys) +
                                 sizeof(header_t) +
                                 sizeof(void*) * table_size);

    return pkeys;
}

/* ------------------------------------------------------------------------- */
static void
ksfree(void** pkeys, hash_t table_size)
{
    (void)table_size;
    FREE(*pkeys);
    FREE(pkeys);
}

/* ------------------------------------------------------------------------- */
static void
load(hash_t home, void** pkeys, void** data, size_t* len)
{
    meta_t* meta;

    meta = JUMP_TABLE(*pkeys)[home];
    *len = meta->len;
    *data = meta + 1;
}

/* ------------------------------------------------------------------------- */
static int
store(hash_t home, void** pkeys, const void* data, size_t len)
{
    meta_t* meta;
    void* keys = *pkeys;

    assert(data != NULL);

    if (HEADER(keys)->capacity - HEADER(keys)->used < len + sizeof(meta_t))
    {
        keys = MALLOC(HEADER(keys)->capacity * 2);
        if (keys == NULL)
            return -1;
        memcpy(keys, *pkeys, HEADER(*pkeys)->capacity);

        HEADER(keys)->capacity = HEADER(*pkeys)->capacity * 2;
        HEADER(keys)->free = (uint8_t*)HEADER(*pkeys)->free - (uint8_t*)(*pkeys) + (uint8_t*)keys;
        FREE(*pkeys);
        *pkeys = keys;
    }

    meta = HEADER(keys)->free;
    meta->len = len;
    memcpy(meta + 1, data, len);
    HEADER(keys)->free = ((uint8_t*)(meta + 1)) + len;
    HEADER(keys)->used += sizeof(meta_t) + len;
    JUMP_TABLE(keys)[home] = meta;

    return 0;
}

/* ------------------------------------------------------------------------- */
static void
erase(hash_t home, void** pkeys)
{
    JUMP_TABLE(*pkeys)[home] = NULL;
    /* TODO move data down */
}

/* ------------------------------------------------------------------------- */
static hash_t
find_existing(hash_t key,
              const hash_t* table, void** pkeys, hash_t table_size,
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
            meta_t* meta = JUMP_TABLE(*pkeys)[home];
            if (meta->len == len && memcmp(meta + 1, data, len) == 0)
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
         const hash_t* table, void** pkeys, hash_t table_size,
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
            meta_t* meta = JUMP_TABLE(*pkeys)[home];
            if (meta->len == len && memcmp(meta + 1, data, len) == 0)
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
key_store_t contiguous_key_store = {
    ksalloc,
    ksfree,
    load,
    store,
    erase,
    find_existing,
    find_new
};

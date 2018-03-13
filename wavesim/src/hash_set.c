#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

#define SLOT_UNUSED (hash_t)0

static void** malloc_key_store(hash_t table_size)
{
    void** key_store = MALLOC(sizeof(void*) * table_size);
    if (key_store == NULL)
        return NULL;
    memset(key_store, 0, sizeof(void*) * table_size);
    return key_store;
}
static void free_key_store(void** key_store, hash_t table_size)
{
    hash_t i;
    for (i = 0; i != table_size; ++i)
        if (key_store[i] != NULL)
            FREE(key_store[i]);
    FREE(key_store);
}
static hash_t insert_key(hash_t key,
                         hash_t* table, void** key_store, hash_t table_size,
                         const void* data, size_t len)
{
    hash_t i;
    hash_t home = key % table_size;
    for (i = 1; i != table_size + 1; ++i)
    {
        if (table[home] == SLOT_UNUSED)
            break;
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
static hash_t search_key(hash_t key,
                         hash_t* table, void** key_store, hash_t table_size,
                         const void* data, size_t len)
{
    hash_t i;
    hash_t home = key % table_size;
    for (i = 1; i != table_size+1; ++i)
    {
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
static int resize_rehash(hash_set_t* hs, hash_t new_size)
{
    hash_t i, key;
    void** new_key_store;
    hash_t* new_table;

    new_table = MALLOC(sizeof(hash_t) * new_size);
    if (new_table == NULL)
        goto table_alloc_failed;
    memset(new_table, 0, sizeof(hash_t) * new_size); /* NOTE: Only works if SLOT_UNUSED is 0 */

    new_key_store = malloc_key_store(new_size);
    if (new_key_store == NULL)
        goto keys_alloc_failed;

    for (i = 0; i != hs->table_size; ++i)
    {
        void* data;
        size_t len;

        /* Key is stored in the table */
        key = hs->table[i];
        if (key == SLOT_UNUSED)
            continue;

        /*
         * The un-hashed key (data) is stored in they "keys" array, where the
         * first few bytes store the length of the data proceeding. Load the
         * length and pointer to the data.
         */
        memcpy(&len, hs->key_store[i], sizeof(size_t));
        data = (uint8_t*)hs->key_store[i] + sizeof(size_t);


        /* Insert into new table and keys array */
        if (insert_key(key, new_table, new_key_store, new_size, data, len) == HASH_SET_OUT_OF_MEMORY)
            goto insert_key_failed;
    }

    /* Swap pointers to new arrays and clean up old ones */
    FREE(hs->table);
    free_key_store(hs->key_store, hs->table_size);
    hs->table = new_table;
    hs->key_store = new_key_store;
    hs->table_size = new_size;

    return 0;

    insert_key_failed  : free_key_store(new_key_store, new_size);
    keys_alloc_failed  : FREE(new_table);
    table_alloc_failed : return -1;
}

/* ------------------------------------------------------------------------- */
wsret
hash_set_create(hash_set_t** hs, uint8_t flags)
{
    wsret result;

    *hs = MALLOC(sizeof(hash_set_t));
    if (*hs == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    if ((result = hash_set_construct(*hs, flags)) != WS_OK)
        return result;
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
hash_set_destroy(hash_set_t* hs)
{
    hash_set_destruct(hs);
    FREE(hs);
}

/* ------------------------------------------------------------------------- */
wsret
hash_set_construct(hash_set_t* hs, uint8_t flags)
{
    hs->hash = hash_jenkins_oaat;
    hs->table_size = 8;
    hs->slots_used = 0;
    hs->table = MALLOC(sizeof(hash_t) * hs->table_size);
    if (hs->table == NULL)
        goto table_alloc_failed;
    memset(hs->table, 0, sizeof(hash_t) * hs->table_size); /* NOTE: Only works if SLOT_UNUSED is 0 */

    hs->key_store = malloc_key_store(hs->table_size);
    if (hs->key_store == NULL)
        goto keys_alloc_failed;

    return WS_OK;

    keys_alloc_failed  : FREE(hs->table);
    table_alloc_failed : WSRET(WS_ERR_OUT_OF_MEMORY);
    (void)flags;
}

/* ------------------------------------------------------------------------- */
void
hash_set_destruct(hash_set_t* hs)
{
    hash_t i;
    for (i = 0; i != hs->table_size; ++i)
        if (hs->key_store[i])
            FREE(hs->key_store[i]);

    FREE(hs->table);
    FREE(hs->key_store);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_add(hash_set_t* hs, const void* data, size_t len)
{
    hash_t key;

    /*
     * If we reach a load factor of 80% or more, resize the table and rehash
     * everything.
     */
    if ((100*hs->slots_used) / hs->table_size > 80)
        resize_rehash(hs, hs->table_size * 2);

    key = hs->hash(data, len);
    key = insert_key(key, hs->table, hs->key_store, hs->table_size, data, len);
    if (key != HASH_SET_OUT_OF_MEMORY && key != HASH_SET_ERROR)
        hs->slots_used++;
    return key;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_find(const hash_set_t* hs, const void* data, size_t len)
{
    hash_t key;

    key = hs->hash(data, len);
    return search_key(key, hs->table, hs->key_store, hs->table_size, data, len);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_remove(hash_set_t* hs, const void* data, size_t len)
{
    (void)hs;
    (void)data;
    (void)len;
    return HASH_SET_ERROR;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_add_str(hash_set_t* hs, const char* str)
{
    return hash_set_add(hs, str, strlen(str) + 1);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_find_str(const hash_set_t* hs, const char* str)
{
    return hash_set_find(hs, str, strlen(str) + 1);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_remove_str(hash_set_t* hs, const char* str)
{
    return hash_set_remove(hs, str, strlen(str) + 1);
}

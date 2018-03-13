#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
/*
 * Need to account for the possibility that our has function will produce
 * reserved values. In this case, return a value that is not reserved, but
 * predictable.
 */
static hash_t
hash_wrapper(const hash_set_t* hs, const void* data, size_t len)
{
    hash_t hash = hs->hash(data, len);
    if (hash == SLOT_UNUSED || hash == SLOT_TOMBSTONE)
        return 2;
    return hash;
}

/* ------------------------------------------------------------------------- */
static int
resize_rehash(hash_set_t* hs, hash_t new_size)
{
    hash_t i, key;
    void** new_key_store;
    hash_t* new_table;

    new_table = MALLOC(sizeof(hash_t) * new_size);
    if (new_table == NULL)
        goto table_alloc_failed;
    memset(new_table, 0, sizeof(hash_t) * new_size); /* NOTE: Only works if SLOT_UNUSED is 0 */

    new_key_store = hs->key_store.alloc(new_size);
    if (new_key_store == NULL)
        goto keys_alloc_failed;

    for (i = 0; i != hs->table_size; ++i)
    {
        void* data;
        size_t len;

        /* Key is stored in the table */
        key = hs->table[i];
        if (key == SLOT_UNUSED || key == SLOT_TOMBSTONE)
            continue;

        /*
         * The un-hashed key (data) is stored in they "keys" array, where the
         * first few bytes store the length of the data proceeding. Load the
         * length and pointer to the data.
         */
        memcpy(&len, hs->keys[i], sizeof(size_t));
        data = (uint8_t*)hs->keys[i] + sizeof(size_t);

        /* Insert into new table and keys array */
        if (hs->key_store.add(key, new_table, new_key_store, new_size, data, len) == HASH_SET_OUT_OF_MEMORY)
            goto insert_key_failed;
    }

    /* Swap pointers to new arrays and clean up old ones */
    FREE(hs->table);
    hs->key_store.free(hs->keys, hs->table_size);
    hs->table = new_table;
    hs->keys = new_key_store;
    hs->table_size = new_size;

    return 0;

    insert_key_failed  :
    hs->key_store.free(new_key_store, new_size);
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

    if (flags & HM_INSERTION_HEAVY)
    {
        hs->key_store = generic_key_store;
    }
    else
    {
        hs->key_store = generic_key_store;
    }

    hs->keys = hs->key_store.alloc(hs->table_size);
    if (hs->keys == NULL)
        goto keys_alloc_failed;

    return WS_OK;

    keys_alloc_failed  : FREE(hs->table);
    table_alloc_failed : WSRET(WS_ERR_OUT_OF_MEMORY);
}

/* ------------------------------------------------------------------------- */
void
hash_set_destruct(hash_set_t* hs)
{
    hash_t i;
    for (i = 0; i != hs->table_size; ++i)
        if (hs->keys[i])
            FREE(hs->keys[i]);

    FREE(hs->table);
    FREE(hs->keys);
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

    key = hash_wrapper(hs, data, len);
    key = hs->key_store.add(key, hs->table, hs->keys, hs->table_size, data, len);
    if (key != HASH_SET_OUT_OF_MEMORY && key != HASH_SET_ERROR)
        hs->slots_used++;
    return key;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_find(const hash_set_t* hs, const void* data, size_t len)
{
    hash_t key;

    key = hash_wrapper(hs, data, len);
    return hs->key_store.find(key, hs->table, hs->keys, hs->table_size, data, len);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_remove(hash_set_t* hs, const void* data, size_t len)
{
    hash_t key = hash_wrapper(hs, data, len);
    hash_t home = hs->key_store.find(key, hs->table, hs->keys, hs->table_size, data, len);
    if (home == HASH_SET_ERROR)
        return HASH_SET_ERROR;

    hs->key_store.remove(home, hs->table, hs->keys);
    return home;
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

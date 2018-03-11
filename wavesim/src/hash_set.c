#include "wavesim/hash_set.h"
#include "wavesim/memory.h"
#include <string.h>

#define HASH_COLLISION (hash_t)-1
#define SLOT_UNUSED (hash_t)0

/* ------------------------------------------------------------------------- */
static void
generic_ks_construct(struct hash_set_key_store_t* ks)
{
    ks->buffer = NULL,
    ks->len = 0;
}
static void
generic_ks_destruct(struct hash_set_key_store_t* ks)
{
    size_t i;
    void** buffer = (void**)ks->buffer;
    for (i = 0; i != ks->len; ++i)
    {
        if (buffer[i] != NULL)
            FREE(buffer[i]);
    }
}
static int
generic_ks_bucket_resized(struct hash_set_key_store_t* ks, hash_t bucket_size, hash_func hash)
{
    void** new_buffer = MALLOC((sizeof(void*) + sizeof(size_t)) * bucket_size);
    if (new_buffer == NULL)
        return -1;

    if (ks->buffer != NULL)
    {
        memcpy(new_buffer, ks->buffer, ks->len);
        FREE(ks->buffer);
    }

    ks->buffer = (uint8_t*)new_buffer;

    return 0;
}
static int
generic_ks_set(hash_set_key_store_t* ks, hash_t pos, const void* data, size_t len)
{
    void** buffer = (void**)ks->buffer;
    void* new_data = MALLOC(len);
    if (new_data == NULL)
        return -1;

    if (buffer[pos] != NULL)
        FREE(buffer[pos]);

    buffer[pos] = new_data;
    memcpy(new_data, data, len);

    return 0;
}
static int
generic_ks_compare(hash_set_key_store_t* ks, hash_t pos, const void* data, size_t len)
{
    void** buffer = (void**)ks->buffer;
    buffer[pos];
}
static void
generic_ks_remove(hash_set_key_store_t* ks, hash_t pos)
{
    void** buffer = (void**)ks->buffer;
    if (buffer[pos] != NULL)
    {
        FREE(buffer[pos]);
        buffer[pos] = NULL;
    }
}

/* ------------------------------------------------------------------------- */
wsret
hash_set_create(hash_set_t** hs, uint8_t flags)
{
    *hs = MALLOC(sizeof(hash_set_t));
    if (*hs == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);

    hash_set_construct(*hs, flags);
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
void
hash_set_construct(hash_set_t* hs, uint8_t flags)
{
    hs->table = NULL;
    hs->table_size = 0;

    hs->hash = hash_jenkins_oaat;

    if (flags & HM_INSERTION_HEAVY)
    {
        hs->key_store.construct      = generic_ks_construct;
        hs->key_store.destruct       = generic_ks_destruct;
        hs->key_store.bucket_resized = generic_ks_bucket_resized;
        hs->key_store.set            = generic_ks_set;
        hs->key_store.compare        = generic_ks_compare;
        hs->key_store.remove         = generic_ks_remove;
    }
    else
    {
        hs->key_store.construct      = generic_ks_construct;
        hs->key_store.destruct       = generic_ks_destruct;
        hs->key_store.bucket_resized = generic_ks_bucket_resized;
        hs->key_store.set            = generic_ks_set;
        hs->key_store.compare        = generic_ks_compare;
        hs->key_store.remove         = generic_ks_remove;
    }

    hs->key_store.construct(&hs->key_store);
}

/* ------------------------------------------------------------------------- */
void
hash_set_destruct(hash_set_t* hs)
{
    if (hs->table)
        FREE(hs->table);
    hs->key_store.destruct(&hs->key_store);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_add(hash_set_t* hs, const void* data, size_t len)
{
    hash_t key = hs->hash(data, len);
    hash_t home = key % hs->table_size;
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_find(const hash_set_t* hs, const void* data, size_t len)
{
    hash_t key = hs->hash(data, len);
    hash_t home = key % hs->table_size;

    if (hs->table[home] == key)
        return home;

    if (hs->table[home] == HASH_COLLISION)
    {
        hash_t i;
        hash_t probe = home;
        for (i = 1; i != hs->table_size; ++i)
        {
            probe += (i*i+i) >> 1;
            if (memcmp(hs->key_store.get(&hs->key_store, hs->table[probe])))
        }
    }
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_remove(hash_set_t* hs, const void* data, size_t len)
{

}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_add_str(hash_set_t* hs, const char* str)
{

}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_find_str(const hash_set_t* hs, const char* str)
{

}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API hash_t
hash_set_remove_str(hash_set_t* hs, const char* data)
{

}

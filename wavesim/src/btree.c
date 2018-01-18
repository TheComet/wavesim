#include "wavesim/btree.h"
#include "wavesim/memory.h"
#include <assert.h>
#include <string.h>

const uint32_t BTREE_VECTOR_INVALID_HASH = (uint32_t)-1;

/* ------------------------------------------------------------------------- */
btree_t*
btree_create(void)
{
    btree_t* btree = MALLOC(sizeof *btree);
    if (btree == NULL)
        return NULL;
    btree_construct(btree);
    return btree;
}

/* ------------------------------------------------------------------------- */
void
btree_construct(btree_t* btree)
{
    assert(btree);
    vector_construct(&btree->vector, sizeof( btree_hash_value_t ));
}

/* ------------------------------------------------------------------------- */
void
btree_destroy(btree_t* btree)
{
    assert(btree);
    btree_clear_free(btree);
    FREE(btree);
}

/* ------------------------------------------------------------------------- */
/* algorithm taken from GNU GCC stdlibc++'s lower_bound function, line 2121 in stl_algo.h */
/* https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a02014.html */
static btree_hash_value_t*
btree_find_lower_bound(const btree_t* btree, uint32_t hash)
{
	intptr_t half;
    btree_hash_value_t* middle;
    btree_hash_value_t* data;
	intptr_t len;

    assert(btree);

    data = ( btree_hash_value_t*)btree->vector.data;
    len = btree->vector.count;

    /* if (the vector has no data, return NULL */
    if (!len)
        return NULL;

    while (len > 0)
    {
        half = len >> 1;
        middle = data + half;
        if (middle->hash < hash)
        {
            data = middle;
            ++data;
            len = len - half - 1;
        }
        else
            len = half;
    }

    /* if ("data" is pointing outside of the valid elements in the vector, also return NULL */
    if ((intptr_t)data >= (intptr_t)btree->vector.data + btree->vector.count * btree->vector.element_size)
        return NULL;
    else
        return data;
}

/* ------------------------------------------------------------------------- */
int
btree_insert(btree_t* btree, uint32_t hash, void* value)
{
    btree_hash_value_t* emplaced_data;
    btree_hash_value_t* lower_bound;

    assert(btree);

    /* don't insert reserved hashes */
    if (hash == BTREE_VECTOR_INVALID_HASH)
        return -1;

    /* lookup location in btree to insert */
    lower_bound = btree_find_lower_bound(btree, hash);
    if (lower_bound && lower_bound->hash == hash)
        return 1;

    /* either push back or insert, depending on whether there is already data
     * in the btree */
    if (!lower_bound)
        emplaced_data = (btree_hash_value_t*)vector_emplace(&btree->vector);
    else
        emplaced_data = vector_insert_emplace(&btree->vector,
                          lower_bound - (btree_hash_value_t*)btree->vector.data);

    if (!emplaced_data)
        return -1;

    memset(emplaced_data, 0, sizeof *emplaced_data);
    emplaced_data->hash = hash;
    emplaced_data->value = value;

    return 0;
}

/* ------------------------------------------------------------------------- */
void
btree_set(btree_t* btree, uint32_t hash, void* value)
{
    btree_hash_value_t* data;

    assert(btree);

    data = btree_find_lower_bound(btree, hash);
    if (data && data->hash == hash)
        data->value = value;
}

/* ------------------------------------------------------------------------- */
void*
btree_find(const btree_t* btree, uint32_t hash)
{
    void** result = btree_find_ptr(btree, hash);
    return result == NULL ? NULL : *result;
}

/* ------------------------------------------------------------------------- */
void**
btree_find_ptr(const btree_t* btree, uint32_t hash)
{
    btree_hash_value_t* data;

    assert(btree);

    data = btree_find_lower_bound(btree, hash);
    if (!data || data->hash != hash)
        return NULL;
    return &data->value;
}

/* ------------------------------------------------------------------------- */
uint32_t
btree_find_element(const btree_t* btree, const void* value)
{
    assert(btree);

    VECTOR_FOR_EACH(&btree->vector, btree_hash_value_t, kv)
        if (kv->value == value)
            return kv->hash;
    VECTOR_END_EACH
    return BTREE_VECTOR_INVALID_HASH;
}

/* ------------------------------------------------------------------------- */
void*
btree_get_any_element(const btree_t* btree)
{
    btree_hash_value_t* kv;
    assert(btree);
    kv = ( btree_hash_value_t*)vector_back(&btree->vector);
    if (kv)
        return kv->value;
    return NULL;
}

/* ------------------------------------------------------------------------- */
int
btree_hash_exists(btree_t* btree, uint32_t hash)
{
    btree_hash_value_t* data;

    assert(btree);

    data = btree_find_lower_bound(btree, hash);
    if (data && data->hash == hash)
        return 0;
    return -1;
}

/* ------------------------------------------------------------------------- */
uint32_t
btree_find_unused_hash(btree_t* btree)
{
    uint32_t i = 0;

    assert(btree);

    BTREE_FOR_EACH(btree, void, key, value)
        if (i != key)
            break;
        ++i;
    BTREE_END_EACH
    return i;
}

/* ------------------------------------------------------------------------- */
void*
btree_erase(btree_t* btree, uint32_t hash)
{
    void* value;
    btree_hash_value_t* data;

    assert(btree);

    data = btree_find_lower_bound(btree, hash);
    if (!data || data->hash != hash)
        return NULL;

    value = data->value;
    vector_erase_element(&btree->vector, (DATA_POINTER_TYPE*)data);
    return value;
}

/* ------------------------------------------------------------------------- */
void*
btree_erase_element(btree_t* btree, void* value)
{
    void* data;
    uint32_t hash;

    assert(btree);

    hash = btree_find_element(btree, value);
    if (hash == BTREE_VECTOR_INVALID_HASH)
        return NULL;

    data = btree_find_lower_bound(btree, hash);
    vector_erase_element(&btree->vector, (DATA_POINTER_TYPE*)data);

    return value;
}

/* ------------------------------------------------------------------------- */
void
btree_clear(btree_t* btree)
{
    assert(btree);
    vector_clear(&btree->vector);
}

/* ------------------------------------------------------------------------- */
void btree_clear_free(btree_t* btree)
{
    assert(btree);
    vector_clear_free(&btree->vector);
}

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "wavesim/vector.h"
#include "wavesim/memory.h"

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

/*!
 * @brief Expands the underlying memory.
 *
 * This implementation will expand the memory by a factor of 2 each time this
 * is called. All elements are copied into the new section of memory.
 * @param[in] insertion_index Set to -1 if (no space should be made for element
 * insertion. Otherwise this parameter specifies the index of the element to
 * "evade" when re-allocating all other elements.
 * @param[in] target_size If set to 0, target size is calculated automatically.
 * Otherwise the vector will expand to the specified target size.
 * @note No checks are performed to make sure the target size is large enough.
 */
static intptr_t
vector_expand(vector_t *vector,
              intptr_t insertion_index,
              intptr_t target_size);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
vector_t*
vector_create(const intptr_t element_size)
{
    vector_t* vector = MALLOC(sizeof(vector_t));
    if (vector == NULL)
        return NULL;
    vector_construct(vector, element_size);
    return vector;
}

/* ------------------------------------------------------------------------- */
void
vector_construct(vector_t* vector, const intptr_t element_size)
{
    assert(vector);
    memset(vector, 0, sizeof(vector_t));
    vector->element_size = element_size;
}

/* ------------------------------------------------------------------------- */
void
vector_destroy(vector_t* vector)
{
    assert(vector);
    vector_clear_free(vector);
    FREE(vector);
}

/* ------------------------------------------------------------------------- */
void
vector_clear(vector_t* vector)
{
    assert(vector);
    /*
     * No need to free or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

/* ------------------------------------------------------------------------- */
void
vector_clear_free(vector_t* vector)
{
    assert(vector);

    if (vector->data)
        FREE(vector->data);

    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

/* ------------------------------------------------------------------------- */
intptr_t
vector_resize(vector_t* vector, intptr_t size)
{
    intptr_t result = 0;

    assert(vector);

    if (vector->count < size)
        result = vector_expand(vector, -1, size);
    vector->count = size;

    return result;
}

/* ------------------------------------------------------------------------- */
void*
vector_emplace(vector_t* vector)
{
    void* data;

    assert(vector);

    if (vector->count == vector->capacity)
        if (vector_expand(vector, -1, 0) < 0)
            return NULL;
    data = vector->data + (vector->element_size * vector->count);
    ++(vector->count);
    return data;
}

/* ------------------------------------------------------------------------- */
intptr_t
vector_push(vector_t* vector, void* data)
{
    void* emplaced;

    assert(vector);
    assert(data);

    emplaced = vector_emplace(vector);
    if (!emplaced)
        return VECTOR_ERROR;
    memcpy(emplaced, data, vector->element_size);
    return vector_count(vector) - 1;
}

/* ------------------------------------------------------------------------- */
intptr_t
vector_push_vector(vector_t* vector, const vector_t* source_vector)
{
    assert(vector);
    assert(source_vector);

    /* make sure element sizes are equal */
    if (vector->element_size != source_vector->element_size)
        return VECTOR_ERROR;

    /* make sure there's enough space in the target vector */
    if (vector->count + source_vector->count > vector->capacity)
        if (vector_expand(vector, -1, vector->count + source_vector->count) < 0)
            return VECTOR_ERROR;

    /* copy data */
    memcpy(vector->data + (vector->count * vector->element_size),
           source_vector->data,
           source_vector->count * vector->element_size);
    vector->count += source_vector->count;

    return 0;
}

/* ------------------------------------------------------------------------- */
void*
vector_pop(vector_t* vector)
{
    assert(vector);

    if (!vector->count)
        return NULL;

    --(vector->count);
    return vector->data + (vector->element_size * vector->count);
}

/* ------------------------------------------------------------------------- */
void*
vector_back(const vector_t* vector)
{
    assert(vector);

    if (!vector->count)
        return NULL;

    return vector->data + (vector->element_size * (vector->count - 1));
}

/* ------------------------------------------------------------------------- */
void*
vector_insert_emplace(vector_t* vector, intptr_t index)
{
    assert(vector);

    /*
     * Normally the last valid index is (capacity-1), but in this case it's valid
     * because it's possible the user will want to insert at the very end of
     * the vector.
     */
    if (index > vector->count)
        return NULL;

    /* re-allocate? */
    if (vector->count == vector->capacity)
    {
        if (vector_expand(vector, index, 0) < 0)
            return NULL;
    }
    else
    {
        /* shift all elements up by one to make space for insertion */
        intptr_t offset = vector->element_size * index;
        intptr_t total_size = vector->count * vector->element_size;
        void* src = (void*)((intptr_t)vector->data + offset);
        void* dst = (void*)((intptr_t)vector->data + offset + vector->element_size);
        intptr_t bytes = total_size - offset;
        memmove(dst, src, bytes);
    }

    /* return pointer to memory of new element */
    ++vector->count;
    return (void*)(vector->data + index * vector->element_size);
}

/* ------------------------------------------------------------------------- */
int
vector_insert(vector_t* vector, intptr_t index, void* data)
{
    void* emplaced;

    assert(vector);
    assert(data);

    emplaced = vector_insert_emplace(vector, index);
    if (!emplaced)
        return -1;
    memcpy(emplaced, data, vector->element_size);
    return 0;
}

/* ------------------------------------------------------------------------- */
void
vector_erase_index(vector_t* vector, intptr_t index)
{
    assert(vector);

    if (index >= vector->count)
        return;

    if (index == vector->count - 1)
        /* last element doesn't require memory shifting, just pop it */
        vector_pop(vector);
    else
    {
        /* shift memory right after the specified element down by one element */
        intptr_t offset = vector->element_size * index;  /* offset to the element being erased in bytes */
        intptr_t total_size = vector->element_size * vector->count; /* total current size in bytes */
        memmove((void*)((intptr_t)vector->data + offset),   /* target is to overwrite the element specified by index */
                (void*)((intptr_t)vector->data + offset + vector->element_size),    /* copy beginning from one element ahead of element to be erased */
                total_size - offset - vector->element_size);     /* copying number of elements after element to be erased */
        --vector->count;
    }
}

/* ------------------------------------------------------------------------- */
void
vector_erase_element(vector_t* vector, void* element)
{
    uintptr_t last_element;

    assert(vector);
    last_element = (uintptr_t)vector->data + (vector->count-1) * vector->element_size;
    assert(element);
    assert((uintptr_t)element >= (uintptr_t)vector->data);
    assert((uintptr_t)element <= (uintptr_t)last_element);

    if (element != (void*)last_element)
    {
        memmove(element,    /* target is to overwrite the element */
                (void*)((uintptr_t)element + vector->element_size), /* read everything from next element */
                last_element - (uintptr_t)element);
    }
    --vector->count;
}

/* ------------------------------------------------------------------------- */
void*
vector_get_element(const vector_t* vector, intptr_t index)
{
    assert(vector);

    if (index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static intptr_t
vector_expand(vector_t *vector,
                      intptr_t insertion_index,
                      intptr_t target_count)
{
    intptr_t new_count;
    DATA_POINTER_TYPE* old_data;
    DATA_POINTER_TYPE* new_data;

    /* expand by factor 2, or adopt target count if (it is not 0 */
    if (target_count)
        new_count = target_count;
    else
        new_count = vector->capacity << 1;

    /*
     * If vector hasn't allocated anything yet, just allocated the requested
     * amount of memory and return immediately.
     */
    if (!vector->data)
    {
        new_count = (new_count == 0 ? 2 : new_count);
        vector->data = MALLOC(new_count * vector->element_size);
        if (!vector->data)
            return -1;
        vector->capacity = new_count;
        return 0;
    }

    /* prepare for reallocating data */
    old_data = vector->data;
    new_data = (DATA_POINTER_TYPE*)MALLOC(new_count * vector->element_size);
    if (!new_data)
        return -1;

    /* if (no insertion index is required, copy all data to new memory */
    if (insertion_index == -1 || insertion_index >= new_count)
        memcpy(new_data, old_data, vector->count * vector->element_size);

    /* keep space for one element at the insertion index */
    else
    {
        /* copy old data up until right before insertion offset */
        intptr_t offset = vector->element_size * insertion_index;
        intptr_t total_size = vector->element_size * vector->count;
        memcpy(new_data, old_data, offset);
        /* copy the remaining amount of old data shifted one element ahead */
        memcpy((void*)((intptr_t)new_data + offset + vector->element_size),
               (void*)((intptr_t)old_data + offset),
               total_size - offset);
    }

    vector->data = new_data;
    vector->capacity = new_count;
    FREE(old_data);

    return 0;
}

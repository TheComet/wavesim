#include "gmock/gmock.h"
#include "wavesim/vector.h"

#define NAME vector

using namespace testing;

TEST(NAME, init)
{
    vector_t vec;

    vec.capacity = 45;
    vec.count = 384;
    vec.data = (uint8_t*)4859;
    vec.element_size = 183;
    vector_construct(&vec, sizeof(int));

    ASSERT_THAT(vec.capacity, Eq(0u));
    ASSERT_THAT(vec.count, Eq(0u));
    ASSERT_THAT(vec.data, IsNull());
    ASSERT_THAT(vec.element_size, Eq(sizeof(int)));
}

TEST(NAME, create_initialises_vector)
{
    vector_t* vec = vector_create(sizeof(int));
    ASSERT_THAT(vec->capacity, Eq(0u));
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->data, IsNull());
    ASSERT_THAT(vec->element_size, Eq(sizeof(int)));
    vector_destroy(vec);
}

TEST(NAME, push_increments_count_and_causes_realloc_by_factor_2)
{
    vector_t* vec = vector_create(sizeof(int));
    int x = 9;

    vector_push(vec, &x);
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_THAT(vec->count, Eq(1u));

    vector_push(vec, &x);
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_THAT(vec->count, Eq(2u));

    vector_push(vec, &x);
    ASSERT_THAT(vec->capacity, Eq(4u));
    ASSERT_THAT(vec->count, Eq(3u));

    vector_destroy(vec);
}

TEST(NAME, clear_keeps_buffer_and_resets_count)
{
    vector_t* vec = vector_create(sizeof(int));
    int x = 9;
    vector_push(vec, &x);
    vector_clear(vec);
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_NE((void*)0, vec->data);
    vector_destroy(vec);
}

TEST(NAME, clear_free_deletes_buffer_and_resets_count)
{
    vector_t* vec = vector_create(sizeof(int));
    int x = 9;
    vector_push(vec, &x);
    vector_clear_free(vec);
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(0u));
    ASSERT_THAT(vec->data, IsNull());
    vector_destroy(vec);
}

TEST(NAME, push_emplace_increments_count_and_causes_realloc_by_factor_2)
{
    vector_t* vec = vector_create(sizeof(int));
    vector_emplace(vec);
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_THAT(vec->count, Eq(1u));
    vector_emplace(vec);
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_THAT(vec->count, Eq(2u));
    vector_emplace(vec);
    ASSERT_THAT(vec->capacity, Eq(4u));
    ASSERT_THAT(vec->count, Eq(3u));
    vector_destroy(vec);
}

TEST(NAME, push_empty_vector)
{
    vector_t* vec1 = vector_create(sizeof(int));
    vector_t* vec2 = vector_create(sizeof(int));
    *(int*)vector_emplace(vec1) = 1;
    *(int*)vector_emplace(vec1) = 2;
    vector_push_vector(vec1, vec2);

    ASSERT_THAT(vector_count(vec1), Eq(2u));
    EXPECT_THAT(*(int*)vector_get(vec1, 0), Eq(1));
    EXPECT_THAT(*(int*)vector_get(vec1, 1), Eq(2));

    vector_destroy(vec1);
    vector_destroy(vec2);
}

TEST(NAME, push_vector)
{
    vector_t* vec1 = vector_create(sizeof(int));
    vector_t* vec2 = vector_create(sizeof(int));
    *(int*)vector_emplace(vec1) = 1;
    *(int*)vector_emplace(vec1) = 2;
    *(int*)vector_emplace(vec2) = 3;
    *(int*)vector_emplace(vec2) = 4;
    vector_push_vector(vec1, vec2);

    ASSERT_THAT(vector_count(vec1), Eq(4u));
    EXPECT_THAT(*(int*)vector_get(vec1, 0), Eq(1));
    EXPECT_THAT(*(int*)vector_get(vec1, 1), Eq(2));
    EXPECT_THAT(*(int*)vector_get(vec1, 2), Eq(3));
    EXPECT_THAT(*(int*)vector_get(vec1, 3), Eq(4));

    vector_destroy(vec1);
    vector_destroy(vec2);
}

TEST(NAME, pop_returns_pushed_values)
{
    vector_t* vec = vector_create(sizeof(int));
    int x;
    x = 3; vector_push(vec, &x);
    x = 2; vector_push(vec, &x);
    x = 6; vector_push(vec, &x);
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(6));
    x = 23; vector_push(vec, &x);
    x = 21; vector_push(vec, &x);
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(21));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(23));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(2));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(3));
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(4u));
    ASSERT_NE((void*)0, vec->data);
    vector_destroy(vec);
}

TEST(NAME, pop_returns_push_emplaced_values)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(73));
    *(int*)vector_emplace(vec) = 28;
    *(int*)vector_emplace(vec) = 72;
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(72));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(28));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(24));
    ASSERT_THAT(*(int*)vector_pop(vec), Eq(53));
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(4u));
    ASSERT_NE((void*)0, vec->data);
    vector_destroy(vec);
}

TEST(NAME, pop_empty_vector)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 21;
    vector_pop(vec);
    ASSERT_THAT(vector_pop(vec), IsNull());
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(2u));
    ASSERT_NE((void*)0, vec->data);
    vector_destroy(vec);
}

TEST(NAME, pop_clear_freed_vector)
{
    vector_t* vec = vector_create(sizeof(int));
    ASSERT_THAT(vector_pop(vec), IsNull());
    ASSERT_THAT(vec->count, Eq(0u));
    ASSERT_THAT(vec->capacity, Eq(0u));
    ASSERT_THAT(vec->data, IsNull());
    vector_destroy(vec);
}

TEST(NAME, get_element_random_access)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    vector_destroy(vec);
}

TEST(NAME, popping_preserves_existing_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    *(int*)vector_emplace(vec) = 24;
    vector_pop(vec);
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    vector_destroy(vec);
}

TEST(NAME, erasing_by_index_preserves_existing_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    *(int*)vector_emplace(vec) = 65;
    vector_erase_index(vec, 1);
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(65));
    vector_erase_index(vec, 1);
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(65));
    vector_destroy(vec);
}

TEST(NAME, erasing_by_element_preserves_existing_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    *(int*)vector_emplace(vec) = 65;
    vector_erase_element(vec, vector_get(vec, 1));
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(65));
    vector_erase_element(vec, vector_get(vec, 1));
    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(65));
    vector_destroy(vec);
}

TEST(NAME, inserting_preserves_existing_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    *(int*)vector_emplace(vec) = 65;

    int x = 68;
    vector_insert(vec, 2, &x); // middle insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(65));

    x = 16;
    vector_insert(vec, 0, &x); // beginning insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));

    x = 82;
    vector_insert(vec, 7, &x); // end insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));
    ASSERT_THAT(*(int*)vector_get(vec, 7), Eq(82));

    x = 37;
    vector_insert(vec, 7, &x); // end insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));
    ASSERT_THAT(*(int*)vector_get(vec, 7), Eq(37));
    ASSERT_THAT(*(int*)vector_get(vec, 8), Eq(82));

    vector_destroy(vec);
}

TEST(NAME, insert_emplacing_preserves_existing_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 53;
    *(int*)vector_emplace(vec) = 24;
    *(int*)vector_emplace(vec) = 73;
    *(int*)vector_emplace(vec) = 43;
    *(int*)vector_emplace(vec) = 65;

    *(int*)vector_insert_emplace(vec, 2) = 68; // middle insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(65));

    *(int*)vector_insert_emplace(vec, 0) = 16; // beginning insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));

    *(int*)vector_insert_emplace(vec, 7) = 82; // end insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));
    ASSERT_THAT(*(int*)vector_get(vec, 7), Eq(82));

    *(int*)vector_insert_emplace(vec, 7) = 37; // end insertion

    ASSERT_THAT(*(int*)vector_get(vec, 0), Eq(16));
    ASSERT_THAT(*(int*)vector_get(vec, 1), Eq(53));
    ASSERT_THAT(*(int*)vector_get(vec, 2), Eq(24));
    ASSERT_THAT(*(int*)vector_get(vec, 3), Eq(68));
    ASSERT_THAT(*(int*)vector_get(vec, 4), Eq(73));
    ASSERT_THAT(*(int*)vector_get(vec, 5), Eq(43));
    ASSERT_THAT(*(int*)vector_get(vec, 6), Eq(65));
    ASSERT_THAT(*(int*)vector_get(vec, 7), Eq(37));
    ASSERT_THAT(*(int*)vector_get(vec, 8), Eq(82));

    vector_destroy(vec);
}

TEST(NAME, for_each_no_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    int counter = 0;
    VECTOR_FOR_EACH(vec, int, i)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));
    vector_destroy(vec);
}

TEST(NAME, for_each_reverse_no_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    int counter = 0;
    VECTOR_FOR_EACH_R(vec, int, i)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));
    vector_destroy(vec);
}

TEST(NAME, for_each)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 1;
    *(int*)vector_emplace(vec) = 2;
    *(int*)vector_emplace(vec) = 3;
    int expected_value = 1;
    VECTOR_FOR_EACH(vec, int, i)
        EXPECT_THAT(*i, Eq(expected_value));
        expected_value++;
    VECTOR_END_EACH
    EXPECT_THAT(expected_value, Ne(1));
    vector_destroy(vec);
}

TEST(NAME, for_each_reverse)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 1;
    *(int*)vector_emplace(vec) = 2;
    *(int*)vector_emplace(vec) = 3;
    int expected_value = 3;
    VECTOR_FOR_EACH_R(vec, int, i)
        EXPECT_THAT(*i, Eq(expected_value));
        expected_value--;
    VECTOR_END_EACH
    EXPECT_THAT(expected_value, Ne(1));
    vector_destroy(vec);
}

TEST(NAME, for_each_range_no_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    int counter = 0;
    VECTOR_FOR_EACH_RANGE(vec, int, i, 0, 0)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));
    vector_destroy(vec);
}

TEST(NAME, for_each_range)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 1;
    *(int*)vector_emplace(vec) = 2;
    *(int*)vector_emplace(vec) = 3;
    *(int*)vector_emplace(vec) = 4;
    *(int*)vector_emplace(vec) = 5;
    int expected_value = 2;
    VECTOR_FOR_EACH_RANGE(vec, int, i, 1, 4)
        EXPECT_THAT(*i, Eq(expected_value));
        expected_value++;
    VECTOR_END_EACH
    EXPECT_THAT(expected_value, Eq(5));
    vector_destroy(vec);
}

TEST(NAME, for_each_erase_element)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 1;
    *(int*)vector_emplace(vec) = 2;
    *(int*)vector_emplace(vec) = 3;
    int counter = 1;
    VECTOR_FOR_EACH(vec, int, i)
        if (counter++ == 2)
            VECTOR_ERASE_IN_FOR_LOOP(vec, int, i);
    VECTOR_END_EACH
    EXPECT_THAT(vector_count(vec), Eq(2u));
    EXPECT_THAT(*(int*)vector_get(vec, 0), Eq(1));
    EXPECT_THAT(*(int*)vector_get(vec, 1), Eq(3));
    vector_destroy(vec);
}

TEST(NAME, for_each_erase_all_elements)
{
    vector_t* vec = vector_create(sizeof(int));
    *(int*)vector_emplace(vec) = 1;
    *(int*)vector_emplace(vec) = 2;
    *(int*)vector_emplace(vec) = 3;
    VECTOR_FOR_EACH(vec, int, i)
        VECTOR_ERASE_IN_FOR_LOOP(vec, int, i);
    VECTOR_END_EACH
    EXPECT_THAT(vector_count(vec), Eq(0u));
    vector_destroy(vec);
}

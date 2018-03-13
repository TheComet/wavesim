#include <gmock/gmock.h>
#include "wavesim/hash_set.h"

#define NAME hash_set

using namespace ::testing;

hash_t shitty_hash(const void* data, size_t len)
{
    return 42;
}

TEST(NAME, adding_data_changes_size)
{
    hash_set_t* hs;
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    EXPECT_THAT(hash_set_count(hs), Eq(0u));
    hash_set_add_str(hs, "test");
    EXPECT_THAT(hash_set_count(hs), Eq(1u));
    hash_set_destroy(hs);
}

TEST(NAME, removing_data_changes_size)
{
    hash_set_t* hs;
    hash_t home;
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    EXPECT_THAT((home = hash_set_add_str(hs, "test")), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_remove_str(hs, "test"), Eq(home));
    EXPECT_THAT(hash_set_count(hs), Eq(0u));
    hash_set_destroy(hs);
}

TEST(NAME, removing_nonexistent_data_does_nothing_and_returns_error)
{
    hash_set_t* hs;
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    hash_set_add_str(hs, "test");
    EXPECT_THAT(hash_set_remove_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_count(hs), Eq(1u));
    hash_set_destroy(hs);
}

TEST(NAME, adding_duplicate_data_does_nothing_and_returns_error)
{
    hash_set_t* hs;
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    EXPECT_THAT(hash_set_add_str(hs, "test"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_add_str(hs, "test"), Eq(HASH_SET_ERROR));
    hash_set_destroy(hs);
}

TEST(NAME, rehash_properly_copies_data)
{
    hash_set_t* hs;
    char buf[] = "test0";
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    for (int i = 0; i != 8; ++i)
    {
        hash_set_add_str(hs, buf);
        buf[4]++;
    }
    EXPECT_THAT(hash_set_count(hs), Eq(8u));

    buf[4] = '0';
    for (int i = 0; i != 8; ++i)
    {
        EXPECT_THAT(hash_set_find_str(hs, buf), Ne(HASH_SET_ERROR));
        buf[4]++;
    }

    EXPECT_THAT(hash_set_find_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
    hash_set_destroy(hs);
}

TEST(NAME, hash_collision_when_adding)
{
    hash_set_t* hs;
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    hs->hash = shitty_hash;
    EXPECT_THAT(hash_set_add_str(hs, "test1"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_add_str(hs, "test2"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_count(hs), Eq(2u));
    EXPECT_THAT(hash_set_find_str(hs, "test1"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_find_str(hs, "test2"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_find_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
    hash_set_destroy(hs);
}

TEST(NAME, hash_collisions)
{
    hash_set_t* hs;
    char buf[] = "test0";
    ASSERT_THAT(hash_set_create(&hs, 0), Eq(WS_OK));
    hs->hash = shitty_hash;

    for (int i = 0; i != 8; ++i)
    {
        EXPECT_THAT(hash_set_add_str(hs, buf), Ne(HASH_SET_ERROR));
        buf[4]++;
    }
    EXPECT_THAT(hash_set_count(hs), Eq(8u));

    buf[4] = '0';
    for (int i = 0; i != 8; ++i)
    {
        EXPECT_THAT(hash_set_find_str(hs, buf), Ne(HASH_SET_ERROR));
        buf[4]++;
    }
    EXPECT_THAT(hash_set_find_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
    hash_set_destroy(hs);
}

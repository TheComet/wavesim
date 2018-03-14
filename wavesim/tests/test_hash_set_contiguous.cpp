#include <gmock/gmock.h>
#include "wavesim/hash_set.h"

#define NAME hash_set_contiguous

using namespace ::testing;

static hash_t shitty_hash(const void* data, size_t len)
{
    return 42;
}

class NAME : public Test
{
protected:
    hash_set_t* hs;

public:
    virtual void SetUp()
    {
        ASSERT_THAT(hash_set_create(&hs, HM_INSERTION_HEAVY), Eq(WS_OK));
    }

    virtual void TearDown()
    {
        hash_set_destroy(hs);
    }
};

TEST_F(NAME, adding_data_changes_size)
{
    EXPECT_THAT(hash_set_count(hs), Eq(0u));
    hash_set_add_str(hs, "test");
    EXPECT_THAT(hash_set_count(hs), Eq(1u));
}

TEST_F(NAME, removing_data_changes_size)
{
    hash_t home;
    EXPECT_THAT((home = hash_set_add_str(hs, "test")), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_remove_str(hs, "test"), Eq(home));
    EXPECT_THAT(hash_set_count(hs), Eq(0u));
}

TEST_F(NAME, removing_nonexistent_data_does_nothing_and_returns_error)
{
    hash_set_add_str(hs, "test");
    EXPECT_THAT(hash_set_remove_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_count(hs), Eq(1u));
}

TEST_F(NAME, adding_duplicate_data_does_nothing_and_returns_error)
{
    EXPECT_THAT(hash_set_add_str(hs, "test"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_add_str(hs, "test"), Eq(HASH_SET_ERROR));
}

TEST_F(NAME, rehash_properly_copies_data)
{
    char buf[] = "test0";
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
}

TEST_F(NAME, hash_collision_when_adding)
{
    hs->hash = shitty_hash;
    EXPECT_THAT(hash_set_add_str(hs, "test1"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_add_str(hs, "test2"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_count(hs), Eq(2u));
    EXPECT_THAT(hash_set_find_str(hs, "test1"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_find_str(hs, "test2"), Ne(HASH_SET_ERROR));
    EXPECT_THAT(hash_set_find_str(hs, "nonexistent"), Eq(HASH_SET_ERROR));
}

TEST_F(NAME, hash_collisions)
{
    char buf[] = "test0";
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
}

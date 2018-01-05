#include "gmock/gmock.h"
#include "wavesim/memory.h"

using testing::Eq;

class UtilGlobalEnvironment : public testing::Environment
{
public:
    virtual ~UtilGlobalEnvironment() {}

    virtual void SetUp()
    {
        testing::FLAGS_gtest_death_test_style = "threadsafe";
        memory_init();
    }

    virtual void TearDown()
    {
        EXPECT_THAT(memory_deinit(), Eq(0u)) << "Number of memory leaks";
    }
};

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new UtilGlobalEnvironment);

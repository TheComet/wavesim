#include "gmock/gmock.h"
#include "wavesim/wavesim.h"

using testing::Eq;

class UtilGlobalEnvironment : public testing::Environment
{
public:
    virtual ~UtilGlobalEnvironment();
    virtual void SetUp();
    virtual void TearDown();
};


UtilGlobalEnvironment::~UtilGlobalEnvironment() {}

void UtilGlobalEnvironment::SetUp()
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    wavesim_init();
}

void UtilGlobalEnvironment::TearDown()
{
    EXPECT_THAT(wavesim_deinit(), Eq(0u)) << "Number of memory leaks";
}

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new UtilGlobalEnvironment);

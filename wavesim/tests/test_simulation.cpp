#include "gmock/gmock.h"
#include "wavesim/simulation.h"
#include "wavesim/medium.h"

#define NAME simulation

using namespace ::testing;

TEST(NAME, rectangular_medium)
{
    medium_t* m;
    simulation_t* s;
    ASSERT_THAT(medium_create(&m), Eq(WS_OK));
    ASSERT_THAT(simulation_create(&s), Eq(WS_OK));


    simulation_destroy(s);
}

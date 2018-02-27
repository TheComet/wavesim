#include "gmock/gmock.h"
#include "wavesim/simulation.h"
#include "wavesim/audio_listener.h"
#include "wavesim/audio_source.h"
#include "wavesim/medium.h"

#define NAME simulation

using namespace ::testing;

TEST(NAME, rectangular_medium)
{
    medium_t* m;
    simulation_t* s;
    audio_listener_t* al;
    audio_source_t* as;
    aabb_t bb = aabb(0, 0, 0, 10, 10, 10); // 50 meter box
    ASSERT_THAT(medium_create(&m), Eq(WS_OK));
    ASSERT_THAT(simulation_create(&s), Eq(WS_OK));
    ASSERT_THAT(audio_listener_create(&al), Eq(WS_OK));
    ASSERT_THAT(audio_source_create(&as), Eq(WS_OK));

    medium_add_partition(m, bb.xyzxyz, attribute_default_air());
    simulation_set_medium(s, m);
    simulation_set_audio_source(s, as);
    simulation_set_audio_listener(s, al);

    simulation_set_resolution(s, 15000, 0.1);
    simulation_prepare(s);

    simulation_destroy(s);
    audio_source_destroy(as);
    audio_listener_destroy(al);
    medium_destroy(m);
}

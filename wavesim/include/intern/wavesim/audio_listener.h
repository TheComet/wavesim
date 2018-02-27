#ifndef WAVESIM_AUDIO_LISTENER_H
#define WAVESIM_AUDIO_LISTENER_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"
#include "wavesim/vector.h"

C_BEGIN

typedef struct audio_listener_t
{
    vec3_t position;
    vec3_t velocity;     /* Not to be confused with the sound velocity: This is the
                          * current velocity of the *source* itself. Used to calculate
                          * doppler effect. If the source is not moving through 3D
                          * space then this should be 0.0. */
    vector_t samples;    /* wsreal_t */
    wsreal_t fs;         /* sampling frequency of the audio data in Hz */
    wsreal_t t;          /* Current position in time of the active sample */

} audio_listener_t;

WAVESIM_PRIVATE_API wsret
audio_listener_create(audio_listener_t** al);

WAVESIM_PRIVATE_API void
audio_listener_destroy(audio_listener_t* al);

WAVESIM_PRIVATE_API void
audio_listener_construct(audio_listener_t* al);

WAVESIM_PRIVATE_API void
audio_listener_destruct(audio_listener_t* al);

WAVESIM_PRIVATE_API void
audio_listener_reset(audio_listener_t* al);

WAVESIM_PRIVATE_API wsret
audio_listener_add_sample(audio_listener_t* al, wsreal_t dt, wsreal_t sample);

C_END

#endif /* WAVESIM_AUDIO_LISTENER_H */

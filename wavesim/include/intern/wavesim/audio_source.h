#ifndef WAVESIM_AUDIO_SOURCE_H
#define WAVESIM_AUDIO_SOURCE_H

#include "wavesim/config.h"
#include "wavesim/vec3.h"

C_BEGIN

/*!
 * @brief Defines an audio point-source in 3D space.
 */
typedef struct audio_source_t
{
    vec3_t position;
    vec3_t velocity;   /* Not to be confused with the sound velocity: This is the
                        * current velocity of the *source* itself. Used to calculate
                        * doppler effect. If the source is not moving through 3D
                        * space then this should be 0.0. */
    wsreal_t current_sample;
    wsreal_t* buffer;  /* TODO: Stores all samples for now. Should probably expand
                        * this into its own "audio provider", so multiple sources
                        * can reference the same audio data and also so we have
                        * the option to properly re-sample the signal in case of
                        * a simulation time-step mismatch (e.g. when two audio
                        * sources have different sampling frequencies) or the
                        * option to filter the data before sending it to the
                        * simulator (e.g. volume levels) */
    uint32_t N;          /* total sample count */

    wsreal_t fs;         /* sampling frequency of the audio data in Hz */
    wsreal_t t;          /* Current position in seconds. */
    wsreal_t delay_play; /* After how many seconds the audio should start playing.
                          * Default is 0. */

} audio_source_t;

WAVESIM_PRIVATE_API void
audio_source_construct(audio_source_t* as);

WAVESIM_PRIVATE_API void
audio_source_destruct(audio_source_t* as);

/*!
 * @brief Sets the audio data to be a Dirac-delta function.
 */
WAVESIM_PRIVATE_API wsret
audio_source_set_dirac(audio_source_t* as);

WAVESIM_PRIVATE_API void
audio_source_reset(audio_source_t* as);

/*!
 * @brief Advances audio playback by the specified time step. A new sample is
 * calculated and written to as->current_sample. You can retrieve it after
 * this function returns.
 */
WAVESIM_PRIVATE_API void
audio_source_advance(audio_source_t* as, wsreal_t dt);

C_END

#endif /* WAVESIM_AUDIO_SOURCE_H */

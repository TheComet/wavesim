#include "wavesim/audio_listener.h"

/* ------------------------------------------------------------------------- */
void
audio_listener_construct(audio_listener_t* al)
{
    al->fs = 41000;
    al->t = 0.0;
    vector_construct(&al->samples, sizeof(wsreal_t));
}

/* ------------------------------------------------------------------------- */
void
audio_listener_destruct(audio_listener_t* al)
{
    vector_clear_free(&al->samples);
}

/* ------------------------------------------------------------------------- */
WAVESIM_PRIVATE_API void
audio_listener_reset(audio_listener_t* al)
{
    al->t = 0;
    vector_clear_free(&al->samples);
}

/* ------------------------------------------------------------------------- */
wsret
audio_listener_add_sample(audio_listener_t* al, wsreal_t dt, wsreal_t sample)
{
    wsreal_t* s;
    uint32_t N;

    al->t += dt;
    N = (uint32_t)(al->fs * al->t + 0.5);
    while (vector_count(&al->samples) < N)
    {
        s = vector_emplace(&al->samples);
        if (s == NULL)
            WSRET(WS_ERR_OUT_OF_MEMORY);
        *s = sample;
    }

    return WS_OK;
}

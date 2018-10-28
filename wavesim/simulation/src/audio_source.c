#include "wavesim/memory.h"
#include "wavesim/simulation/audio_source.h"

/* ------------------------------------------------------------------------- */
wsret
audio_source_create(audio_source_t** al)
{
    *al = MALLOC(sizeof(audio_source_t));
    if (*al == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    audio_source_construct(*al);
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
audio_source_destroy(audio_source_t* al)
{
    audio_source_destruct(al);
    FREE(al);
}

/* ------------------------------------------------------------------------- */
void
audio_source_construct(audio_source_t* as)
{
    vec3_set_zero(as->position.xyz);
    vec3_set_zero(as->velocity.xyz);
    as->buffer = NULL;
    as->N = 0;
    as->fs = 41000;
    as->t = 0.0;
    as->delay_play = 0.0;
}

/* ------------------------------------------------------------------------- */
void
audio_source_destruct(audio_source_t* as)
{
    if (as->buffer != NULL)
        FREE(as->buffer);
}

/* ------------------------------------------------------------------------- */
wsret
audio_source_set_dirac(audio_source_t* as)
{
    if (as->buffer != NULL)
        FREE(as->buffer);

    as->buffer = MALLOC(sizeof(wsreal_t)); /* Only need one sample */
    if (as->buffer == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    as->buffer[0] = 1.0;  /* and that sample is 1 */
    as->N = 1;

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
audio_source_reset(audio_source_t* as)
{
    as->t = -as->delay_play;
    audio_source_advance(as, 0.0); /* sets current_sample */
}

/* ------------------------------------------------------------------------- */
void
audio_source_advance(audio_source_t* as, wsreal_t dt)
{
    uint32_t N;
    as->t += dt;

    /* for now, just round to nearest sample */
    N = (uint32_t)(as->fs * as->t + 0.5);
    as->current_sample = as->t >= -0.5 && N < as->N ? as->buffer[N] : 0.0;
}

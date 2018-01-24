#include "wavesim/log.h"
#include "wavesim/memory.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

log_t g_ws_log;

/* ------------------------------------------------------------------------- */
wsret
log_create(log_t** log)
{
    *log = MALLOC(sizeof **log);
    if (*log == NULL)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    log_construct(*log);
    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
log_destroy(log_t* log)
{
    log_destruct(log);
}

/* ------------------------------------------------------------------------- */
void
log_construct(log_t* log)
{
    log->buffer = NULL;
    log->buffer_length = 0;
    log->info = default_info_func;
    log->data = default_data_func;
}

/* ------------------------------------------------------------------------- */
void
log_destruct(log_t* log)
{
    if (log->buffer != NULL)
        FREE(log->buffer);
}

/* ------------------------------------------------------------------------- */
void
log_set_callbacks(log_t* log, log_info_func info, log_data_func data)
{
    log->info = info;
    log->data = data;
}

/* ------------------------------------------------------------------------- */
static void
ws_vlog(log_t* log, const char* fmt, va_list ap, int type)
{
    int len;

    va_list ap2;
    va_copy(ap2, ap);
    len = vsnprintf(NULL, 0, fmt, ap);

    if (len < 0)
        return;

    len += 2; /* newline + null terminator */
    if (len > log->buffer_length)
    {
        char* buf = MALLOC((len + 1) * sizeof(char));
        if (buf == NULL)
        {
            fprintf(stderr, "%s\n", wsret_to_string(WS_ERR_OUT_OF_MEMORY));
            return;
        }
        if (log->buffer != NULL)
            FREE(log->buffer);
        log->buffer = buf;
        log->buffer_length = (len + 1) * sizeof(char);
    }

    vsprintf(log->buffer, fmt, ap);
    strcat(log->buffer, "\n");
    va_end(ap2);

    if (type == 0)
        log->info(log->buffer);
    if (type == 1)
        log->data(log->buffer);
}

/* ------------------------------------------------------------------------- */
void
ws_log_info(log_t* log, const char* fmt, ...)
{
    va_list ap;

    if (log->info == NULL)
        return;

    va_start(ap, fmt);
    ws_vlog(log, fmt, ap, 0);
    va_end(ap);
}

/* ------------------------------------------------------------------------- */
void
ws_log_data(log_t* log, const char* fmt, ...)
{    va_list ap;

    if (log->data == NULL)
        return;

    va_start(ap, fmt);
    ws_vlog(log, fmt, ap, 1);
    va_end(ap);
}

/* ------------------------------------------------------------------------- */
void
default_info_func(const char* msg)
{
    fprintf(stderr, "%s", msg);
}

/* ------------------------------------------------------------------------- */
void
default_data_func(const char* msg)
{
    fprintf(stdout, "%s", msg);
}

#include "wavesim/log.h"
#include "wavesim/memory.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

log_t g_ws_log;

/* ------------------------------------------------------------------------- */
int
log_create(log_t** log)
{
    *log = MALLOC(sizeof **log);
    if (*log == NULL)
        return -1;
    log_construct(*log);
    return 0;
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
vlog(log_t* log, const char* fmt, va_list ap, int type)
{
    int len;

    va_list ap2;
    va_copy(ap2, ap);
    len = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);

    if (len < 0)
        return;

    len += 2; /* newline + null terminator */
    if (len > log->buffer_length)
    {
        char* buf = MALLOC((uintptr_t)len * sizeof(char));
        if (buf == NULL)
        {
            fprintf(stderr, "Ran out of memory while resizing log buffer");
            return;
        }
        if (log->buffer != NULL)
            FREE(log->buffer);
        log->buffer = buf;
        log->buffer_length = (uintptr_t)len * sizeof(char);
    }

    vsprintf(log->buffer, fmt, ap);
    strcat(log->buffer, "\n");

    if (type == 0)
        log->info(log->buffer);
    if (type == 1)
        log->data(log->buffer);
}

/* ------------------------------------------------------------------------- */
void
log_info(log_t* log, const char* fmt, ...)
{
    va_list ap;

    if (log->info == NULL)
        return;

    va_start(ap, fmt);
    vlog(log, fmt, ap, 0);
    va_end(ap);
}

/* ------------------------------------------------------------------------- */
void
log_data(log_t* log, const char* fmt, ...)
{    va_list ap;

    if (log->data == NULL)
        return;

    va_start(ap, fmt);
    vlog(log, fmt, ap, 1);
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

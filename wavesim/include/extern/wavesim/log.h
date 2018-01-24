#ifndef WAVESIM_LOG_H
#define WAVESIM_LOG_H

#include "wavesim/config.h"

C_BEGIN

typedef void (*log_info_func)(const char*);
typedef void (*log_data_func)(const char*);

typedef struct log_t
{
    char* buffer;
    int buffer_length;
    log_info_func info;
    log_data_func data;
} log_t;

WAVESIM_PUBLIC_API extern log_t g_ws_log;

typedef struct llog_t llog_t;
WAVESIM_PUBLIC_API wsret
llog_create(llog_t** log);

WAVESIM_PUBLIC_API void
log_destroy(log_t* log);

WAVESIM_PUBLIC_API void
log_construct(log_t* log);

WAVESIM_PUBLIC_API void
log_destruct(log_t* log);

WAVESIM_PUBLIC_API void
log_set_callbacks(log_t* log, log_info_func info, log_data_func data);

WAVESIM_PUBLIC_API void
ws_log_info(log_t* log, const char* fmt, ...);

WAVESIM_PUBLIC_API void
ws_log_data(log_t* log, const char* fmt, ...);

WAVESIM_PUBLIC_API void
default_info_func(const char* msg);

WAVESIM_PUBLIC_API void
default_data_func(const char* msg);

C_END

#endif /* WAVESIM_LOG_H */

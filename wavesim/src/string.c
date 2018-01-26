#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>
#include "wavesim/memory.h"
#include "wavesim/string.h"

/* ------------------------------------------------------------------------- */
char*
ws_strtok(char* str, const char* delimiter, char** save_ptr)
{
    char* start_ptr;

    if(str) /* non-NULL input string begins a new tokenization */
        *save_ptr = str;

    /* "Optimized" by Oberon */
    /* save_ptr always points to the beginning of the next potential token, or is NULL */
    start_ptr = *save_ptr;
    do
    {
        if (*save_ptr == NULL || (*save_ptr = strpbrk(*save_ptr, delimiter)) == NULL)
            return start_ptr && start_ptr[0] != '\0' ? start_ptr : NULL;
            /* This return check is necessary to avoid returning empty tokens
             * in the case of empty input strings or strings with trailing
             * delimiters */
    } while ((start_ptr == *save_ptr) && (start_ptr = ++(*save_ptr)));

    /* Replaces the delimiter with NULL to trick the token into thinking the
     * string ends early. Store the beginning of the next token */
    **save_ptr = '\0';
    ++(*save_ptr);

    return start_ptr;
}

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

    if(str)
        *save_ptr = str;

    /* "Optimized" by Oberon */
    start_ptr = *save_ptr;
    do
    {
        if (*save_ptr == NULL || (*save_ptr = strpbrk(*save_ptr, delimiter)) == NULL)
            return start_ptr && start_ptr[0] != '\0' ? start_ptr : NULL;
    } while ((start_ptr == *save_ptr) && (start_ptr = ++(*save_ptr)));

    **save_ptr = '\0';
    ++(*save_ptr);

    return start_ptr;
}

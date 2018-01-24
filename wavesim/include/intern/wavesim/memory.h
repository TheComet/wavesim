#ifndef MEMORY_H
#define MEMORY_H

#include "wavesim/config.h"

#ifdef WAVESIM_MEMORY_DEBUGGING
#   define MALLOC malloc_wrapper
#   define FREE   free_wrapper
#else
#   include <stdlib.h>
#   define MALLOC malloc
#   define FREE   free
#endif /* WAVESIM_MEMORY_DEBUGGING */

C_BEGIN

/*!
 * @brief Initialises the memory system.
 *
 * In release mode this does nothing. In debug mode it will initialise
 * memory reports and backtraces, if enabled.
 */
WAVESIM_PRIVATE_API wsret
memory_init(void);

/*!
 * @brief De-initialises the memory system.
 *
 * In release mode this does nothing. In debug mode this will output the memory
 * report and print backtraces, if enabled.
 * @return Returns the difference in malloc/free calls
 */
WAVESIM_PRIVATE_API int
memory_deinit(void);

#ifdef WAVESIM_MEMORY_DEBUGGING
/*!
 * @brief Does the same thing as a normal call to malloc(), but does some
 * additional work to monitor and track down memory leaks.
 */
WAVESIM_PRIVATE_API void*
malloc_wrapper(intptr_t size);

/*!
 * @brief Does the same thing as a normal call to fee(), but does some
 * additional work to monitor and track down memory leaks.
 */
WAVESIM_PRIVATE_API void
free_wrapper(void* ptr);
#endif /* WAVESIM_MEMORY_DEBUGGING */

WAVESIM_PRIVATE_API void
mutated_string_and_hex_dump(void* data, intptr_t size_in_bytes);

C_END

#endif /* MEMORY_H */
